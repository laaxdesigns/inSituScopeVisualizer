/*
 *  OrganismCollection.cpp
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 11/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "OrganismCollection.h"

int OrganismCollection::globID = 0;

OrganismCollection::OrganismCollection() {
	drawT = PATCH;
	bUpdatePos = false;
	bAnimate = true;
	sphereSize = 10.0;
	bReInitSOM = false;
	numSOMIters=300;
	bLearnSOM=false;
	SOMiCount=0;
	//mapSX=mapSY=mapSZ=10;
	mapSX=20;
	mapSY=mapSZ=20;
	SOM_dims=3;
	som.setMapSize(mapSX,mapSY,mapSZ);
	som.setNumIterations(numSOMIters);
	som.setInitialLearningRate(0.1);
	som.setup();
	bDrawAxes = true;
}

void OrganismCollection::setupSOM() {
	ofLog(OF_LOG_VERBOSE,"Changing SOM Setup...");
	vector<double> mins;
	vector<double> maxs;
	int numKeysInUse = 0;
	for (set<string>::iterator it=superKeys.begin(); it!=superKeys.end(); ++it) {
		if (useKeys[(*it)]) {
			mins.push_back(minKeyVals[(*it)]);
			maxs.push_back(maxKeyVals[(*it)]);
			numKeysInUse++;
		}
	}
	som.setFeaturesRange(numKeysInUse, &mins[0], &maxs[0]);
	som.setup();
	bReInitSOM=false;
}

int OrganismCollection::initByHeaderedAttrFile(ofBuffer attr, string imgLoc) {
	
	if (collection.size()>0) {
		ofLog(OF_LOG_WARNING,"The OrganismCollection is not empty... not initing!\n");
		return -1;
	}
	vector<string> keys;
	vector<string> vals;
	string line;
	//Parse first line of ofBuffer to get key-labels for each column
	//first column should be the filename
	if(!attr.isLastLine()) {
		line = attr.getNextLine();
		keys = ofSplitString(line, ",");
		if (keys[0].compare("filename")!=0) {
			ofLog(OF_LOG_WARNING,"First key is %s should be filename\n",keys[0].c_str());
			return -1;
		}
		keys.erase(keys.begin());
	}
	
	
	//Populate an unordered map with all of the files found in the 
	//ofDirectory
	ofDirectory dir;
	int numFiles = dir.listDir(imgLoc);
	map<string,string> imFiles;
	//imFiles.reserve(numFiles);
	for (int i=0; i<numFiles; i++) {
		imFiles[dir.getName(i)]=dir.getPath(i);
	}
	dir.close();
	
	//Read line by line,
	//  1.Load file specified found in the passed ofDir (if exists)
	//  2.Populate attributes for Organism
	//  3.Add it to the collection
	int count = 0;
	while(!attr.isLastLine()) {
		line = attr.getNextLine();
		vals = ofSplitString(line,",");
		if (vals.size()-1!=keys.size()) {
			ofLog(OF_LOG_WARNING,"Line: %s\n Incorrect number of fields...skipping!\n",line.c_str());
			continue;
		}
		string fname = vals[0];
		map<string,string>::const_iterator got = imFiles.find(fname);
		if (got == imFiles.end()) {
			ofLog(OF_LOG_WARNING,"%s not found in the directory...skipping!\n",fname.c_str());
			continue;
		}
		ofImage tmp;
		Organism org;
		tmp.loadImage(got->second);
		org.setTexture(tmp.getPixelsRef());
		org.setFname(got->first);
		
		for (int i=1; i<vals.size(); i++) {
			org.addAttribute(keys[i-1], atof(vals[i].c_str()));
		}
		addOrganism(org);
		count++;
	}
	return count;
}

void OrganismCollection::draw() {
	update();
	if(bLearnSOM) {
		runSOMIter();
	}
	for (int i=0; i<collection.size(); i++) {
		if (collection[i].bDraw) {
			if (drawT==PATCH) {
				collection[i].org.drawOrg(collection[i].position.x,collection[i].position.y,collection[i].position.z);
			}
			else if(drawT==SPHERE) {
				//collection[i].org.texData.bind();
				ofPushStyle();
				ofSetColor(collection[i].color);
				ofSphere(collection[i].position.x, collection[i].position.y, collection[i].position.z, sphereSize);
				ofPopStyle();
				//collection[i].org.texData.bind();
			}
		}
	}
	if(bDrawAxes) {
		axes.draw();
	}
}

vector<string> OrganismCollection::addOrganism(Organism oIn) {
	vector<string> newKeys;
	
	patch newPatch;
	newPatch.org = oIn;
	
	newPatch.position.x = (ofRandom(1.0f)-0.5f)*1000;
	newPatch.position.y = (ofRandom(1.0f)-0.5f)*1000;
	newPatch.position.z = (ofRandom(1.0f)-0.5f)*1000;
	
	newPatch.color.r=255.0f;
	newPatch.color.g=255.0f;
	newPatch.color.b=255.0f;
	newPatch.color.a=180.0f;
	
	newPatch.bDraw=true;
	newPatch.path.push_back(newPatch.position);
	
	newPatch.pid = globID;
	globID++;
	
	//Now go through keys of the Organism and add any not already in 
	//the superset
	map<string,float> tmpUM = newPatch.org.getAttributes();
	map<string,float>::iterator it;
	// show content:
	for ( it=tmpUM.begin() ; it != tmpUM.end(); it++ ) {
		set<string>::const_iterator found = superKeys.find((*it).first);
		if(found == superKeys.end()) {
			//First time key is inserted to min, max are set to value
			superKeys.insert((*it).first);
			newKeys.push_back((*it).first);
			maxKeyVals[(*it).first] = (*it).second;
			minKeyVals[(*it).first] = (*it).second;
			useKeys[(*it).first] = true;
		}
		else {
			if (maxKeyVals[(*it).first] < (*it).second) {
				maxKeyVals[(*it).first] = (*it).second;
			}
			if (minKeyVals[(*it).first] > (*it).second) {
				minKeyVals[(*it).first] = (*it).second;
			}
		}
		
	}
	minKeyThresh=minKeyVals;
	maxKeyThresh=maxKeyVals;
	
	//Finally add the patch to the collection
	collection.push_back(newPatch);
	bReInitSOM = true;
	
	return newKeys;
}

int OrganismCollection::size() {
	return collection.size();
}

vector<string> OrganismCollection::getKeySuperSet() {
	vector<string> ret;
	for (set<string>::iterator it=superKeys.begin(); it!=superKeys.end(); ++it) {
		ret.push_back(*it);
	}
	return ret;
}

void OrganismCollection::update() {
	if(bUpdatePos) {
		bool bDone=true;
		for (int i=0; i<collection.size(); i++) {
			if (collection[i].path.size()>1) {
				bDone=false;
				if(bAnimate) {
				collection[i].position.x += collection[i].velocity[0].x;
				collection[i].position.y += collection[i].velocity[0].y;
				collection[i].position.z += collection[i].velocity[0].z;
				}
				else {
					collection[i].position = collection[i].path[1];
				}
				float deltaX,deltaY,deltaZ;
				deltaX = fabs(collection[i].position.x-collection[i].path[1].x);
				deltaY = fabs(collection[i].position.y-collection[i].path[1].y);
				deltaZ = fabs(collection[i].position.z-collection[i].path[1].z);
				float eps=0.1;
				if (deltaX<eps || deltaY<eps || deltaZ <eps) {
					collection[i].position = collection[i].path[1];
					collection[i].path.erase(collection[i].path.begin());
					collection[i].velocity.erase(collection[i].velocity.begin());
				}
			}
		}
		if (bDone) {
			bUpdatePos=false;
		}
	}
	if (bReInitSOM) {
		setupSOM();
	}
}

void OrganismCollection::setDrawType(DrawType t) {
	drawT = t;
}

void OrganismCollection::randomizePositions() {
	for (int i=0; i<collection.size(); i++) {
		ofPoint tmpP;
		ofVec3f tmpV;
		tmpP.x = (ofRandom(1.0f)-0.5f)*1000;
		tmpP.y = (ofRandom(1.0f)-0.5f)*1000;
		tmpP.z = (ofRandom(1.0f)-0.5f)*1000;
		int tmpS = collection[i].path.size()-1;
		tmpV.x = (tmpP.x-collection[i].path[tmpS].x)/20.0f;
		tmpV.y = (tmpP.y-collection[i].path[tmpS].y)/20.0f;
		tmpV.z = (tmpP.z-collection[i].path[tmpS].z)/20.0f;
		collection[i].path.push_back(tmpP);
		collection[i].velocity.push_back(tmpV);
	}
	bUpdatePos=true;
}

void OrganismCollection::setAnimate(bool b) {
	bAnimate = b;
}

bool OrganismCollection::getAnimate() {
	return bAnimate;
}

float OrganismCollection::getKeyMax(string key) {
	set<string>::const_iterator found = superKeys.find(key);
	if (found == superKeys.end()) {
		return numeric_limits<float>::quiet_NaN();
	}
	else {
		return maxKeyVals[key];
	}

}

float OrganismCollection::getKeyMin(string key) {
	set<string>::const_iterator found = superKeys.find(key);
	if (found == superKeys.end()) {
		return numeric_limits<float>::quiet_NaN();
	}
	else {
		return minKeyVals[key];
	}
}

//Note this will return false both if useKey[key] is set to false
//and if it is not found!
bool OrganismCollection::getKeyUse(string key) {
	set<string>::const_iterator found = superKeys.find(key);
	if (found == superKeys.end()) {
		return false;
	}
	else {
		return useKeys[key];
	}
}

void OrganismCollection::setKeyUse(string k,bool b) {
	map<string,bool>::const_iterator found = useKeys.find(k);
	if (found != useKeys.end()) {
		useKeys[k] = b;
		bReInitSOM = true;
	}
	//printLocs();
}

void OrganismCollection::learnSOM() {
	setupSOM();
	SOMiCount=0;
	bLearnSOM=true;
}

void OrganismCollection::runSOMIter() {
	//Do learning
	int rInd = ofRandom(collection.size()-1);
	vector<double> rAttr;
	rAttr = getInstanceVec(rInd);
	som.updateMap(&rAttr[0]);
	
	SOMiCount++;
	//Update patch positions every 1 iterations
	if (SOMiCount % 10 == 0) {
		ofLog(OF_LOG_VERBOSE,"Finished SOM iter %d",SOMiCount);
		updatePosBySOMMap();
	}
	
	//Check if finished
	if (SOMiCount >= numSOMIters) {
		bLearnSOM = false;
		updatePosBySOMMap();
	}
	
}

vector<double> OrganismCollection::getInstanceVec(int ind) {
	vector<double> rAttr;
	for (set<string>::iterator it=superKeys.begin(); it!=superKeys.end(); ++it) {
		if (useKeys[(*it)]) {
			double tmp = collection[ind].org.getAttribute((*it));
			if(std::isnan(tmp)) {
				//error condition
				ofLog(OF_LOG_WARNING,"Attempting to learn SOM on incomplete example: %s",(*it).c_str());
				tmp = 0.5 * (minKeyVals[(*it)] + maxKeyVals[(*it)]);
			}
			rAttr.push_back(tmp);
		}
	}
	return rAttr;
}

void OrganismCollection::updatePosBySOMMap() {
	vector<double> attrVec;
	ofPoint pt;
	ofVec3f tmpV;
	vector<int> binCount(mapSX);
	
	/*
	 //Code to sort output for more logical display
	vector<vector<double>> wtVecs;
	for(int i=0; i<mapSX; i++) {
		double *curWts = som.getMapAt(i, 0, 0);
		wtVecs.push_back(curWts);
	}
	//sortrows call here
	*/
	 
	for (int i=0; i<collection.size(); i++) {
		attrVec = getInstanceVec(i);
		pt = som.findBestMatchingCell(&attrVec[0]);
		if (SOM_dims<3) {
			binCount[((int)pt.x)]++;
			pt.z = binCount[((int)pt.x)];
			pt.x -= mapSX/2;
			pt.y -= mapSY/2;
			pt.x *= 10;
		}
		else {
			pt.x -= mapSX/2;
			pt.y -= mapSY/2;
			pt.z -= mapSZ/2;
			pt.x *= 10;
			pt.y *= 10;
			pt.z *= 10;
		}
		ofLog(OF_LOG_VERBOSE,"ptx=%d ptz=%f",((int)pt.x),pt.z);
		
		
		
		int tmpS = collection[i].path.size()-1;
		tmpV.x = (pt.x-collection[i].path[tmpS].x)/10.0f;
		tmpV.y = (pt.y-collection[i].path[tmpS].y)/10.0f;
		tmpV.z = (pt.z-collection[i].path[tmpS].z)/10.0f;
		collection[i].path.push_back(pt);
		collection[i].velocity.push_back(tmpV);
	}
	bUpdatePos=true;
}

void OrganismCollection::setKeyThresh(string key, float min, float max) {
	bool bChanged = false;
	if (minKeyThresh.find(key) != minKeyThresh.end()) {
		minKeyThresh[key]=min;
		bChanged=true;
	}
	if (maxKeyThresh.find(key) != maxKeyThresh.end()) {
		maxKeyThresh[key]=max;
		bChanged=true;
	}
	if(bChanged) {
		filter();
	}	
}

void OrganismCollection::filter() {
	for (int i=0; i<collection.size(); i++) {
		collection[i].bDraw = true;
		map<string,float> tmpUM = collection[i].org.getAttributes();
		map<string,float>::iterator it;
		// show content:
		for ( it=tmpUM.begin() ; it != tmpUM.end(); it++ ) {
			map<string,float>::const_iterator found = minKeyThresh.find((*it).first);
			if (found != minKeyThresh.end()) {
				if ((*it).second < (*found).second) {
					collection[i].bDraw=false;
				}
			}
			found = maxKeyThresh.find((*it).first);
			if (found != maxKeyThresh.end()) {
				if ((*it).second > (*found).second) {
					collection[i].bDraw=false;
				}
			}
		}
	}
}

void OrganismCollection::printLocs() {
	ofLog(OF_LOG_VERBOSE,"Collection Size is %d",collection.size());
	for (int i=0; i<collection.size(); i++) {
		ofLog(OF_LOG_VERBOSE,ofToString(collection[i].position));
	}
}

void OrganismCollection::updatePosBySortedKeys(string kx, string kz) {
	float minX,maxX,minZ,maxZ;
	bool valid = true;
	bool useX = false;
	bool useZ = false;
	int xBins=1;
	int zBins=1;
	int xScale=50;
	int zScale =50;
	int nBins = 100;
	
	if (!kx.empty()) {
		minX = getKeyMin(kx);
		maxX = getKeyMax(kx);
		if(std::isnan(minX)||std::isnan(maxX)) {
			valid = false;
		}
		useX = true;
		xBins = nBins;
	}
	if (!kz.empty()) {
		minZ = getKeyMin(kz);
		maxZ = getKeyMax(kz);
		if(std::isnan(minZ)||std::isnan(maxZ)) {
			valid = false;
		}
		useZ = true;
		zBins = nBins;
	}
	
	//Set up axes
	axes.setXAxis(ofPoint((-xBins/2)*xScale,-5,(-zBins/2)*zScale),ofPoint((xBins/2)*xScale,-5,(-zBins/2)*zScale));
	axes.setZAxis(ofPoint((-xBins/2)*xScale,-5,(-zBins/2)*zScale),ofPoint((-xBins/2)*xScale,-5,(zBins/2)*zScale));
	axes.setXLabel(kx);
	axes.setZLabel(kz);
	axes.setXAxisLimits(minX,maxX);
	axes.showXTicks(true);
	axes.setZAxisLimits(minZ,maxZ);
	axes.showZTicks(true);
	
	
	int falseCount=0;
	//Bucket sort organisms along each axis
	if(valid) {
		vector<vector<int> > binCounts(xBins, vector<int>(zBins,0));
		for (int i=0; i<collection.size(); i++) {
			float valX,valZ;
			valX = 0;
			valZ = 0;
			if (useX) {
				valX = collection[i].org.getAttribute(kx);
			}
			if (useZ) {
				valZ = collection[i].org.getAttribute(kz);
			}
			
			if(!std::isnan(valX) && !std::isnan(valZ)) {
				//collection[i].bDraw = true;
				int tmpX=0;
				int tmpZ=0;
				ofPoint tmpP;
				ofVec3f tmpV;
				if (useX) {
					valX = (valX-minX)/(maxX-minX);
					tmpX = valX*(xBins-1);
					tmpP.x = (tmpX-(xBins/2))*xScale;
				}
				if (useZ) {
					valZ = (valZ-minZ)/(maxZ-minZ);
					tmpZ = valZ*(zBins-1);
					tmpP.z = (tmpZ-(zBins/2))*zScale;
				}
				binCounts[tmpX][tmpZ]++;
				
				tmpP.y = binCounts[tmpX][tmpZ] * sphereSize;
				int tmpS = collection[i].path.size()-1;
				tmpV.x = (tmpP.x-collection[i].path[tmpS].x)/10.0f;
				tmpV.y = (tmpP.y-collection[i].path[tmpS].y)/10.0f;
				tmpV.z = (tmpP.z-collection[i].path[tmpS].z)/10.0f;
				collection[i].path.push_back(tmpP);
				collection[i].velocity.push_back(tmpV);
			}
			else {
				falseCount++;
				collection[i].bDraw = false;
			}

		}
	}
	else {
		ofLog(OF_LOG_ERROR,"key data is corrupted in call to updatePoseBySortedKey()");
	}
	ofLog(OF_LOG_VERBOSE,"False Count = %d", falseCount);
	bUpdatePos=true;
}

map<int,ofPoint> OrganismCollection::getPatchLocs() {
	
	map<int,ofPoint> ret;
	for (int i=0; i<collection.size(); i++) {
		if(collection[i].bDraw) {
			ret[collection[i].pid]=collection[i].position;
		}
	}
	return ret;
}

Organism OrganismCollection::getOrganismByID(int idIN) {
	for (int i=0; i<collection.size(); i++) {
		if(collection[i].pid == idIN) {
			return (collection[i].org);
		}
	}
	Organism empty;
	
	return empty;
}

void OrganismCollection::updateColorByKey(string k) {
	
	float keyMax = maxKeyVals[k];
	float keyMin = minKeyVals[k];
	
	//vector<ofVec2f> barLocs;
//	vector<ofFloatColor> barClrs;
//	vector<double> widths;
//	
//	int steps = 100;
//	float delta = (keyMax-keyMin)/(float)steps;
//	float cur = keyMin;
//	float yCur = 20;
//	float xCur = ofGetScreenWidth()-50;
//	while (cur>keyMin) {
//		ofVec2f tmpV;
//		tmpV.set(xCur, yCur);
//		barLocs.push_back(tmpV);
//		float h = ((cur-keyMin)/(keyMax-keyMin))*255;
//		ofFloatColor c = ofFloatColor::fromHsb(h, 255, 255, 255);
//		barClrs.push_back(c);
//		widths.push_back(10);
//		
//		yCur+=5;
//		cur -=  delta;
//	}
//	axes.setColorBar(barLocs,barClrs,widths);
	
	for (int i=0; i<collection.size(); i++) {
		int h = (int)(((collection[i].org.getAttribute(k)-keyMin) / (keyMax-keyMin))*255);
		collection[i].color = ofColor::fromHsb(h, 255, 255, 255);
	}
}

void OrganismCollection::setOrganismToFalse(int idIN) {
	for (int i=0; i<collection.size(); i++) {
		if(collection[i].pid==idIN) {
			collection[i].bDraw = false;
		}
	}
}

void OrganismCollection::showAll() {
	for (int i=0; i<collection.size(); i++) {
		collection[i].bDraw = true;
	}
}

void OrganismCollection::saveVisible(string path) {
	ofFile attrOut;
	attrOut.open(path+"/attributes.csv", ofFile::WriteOnly, true);
	attrOut << "filename";
	for (set<string>::iterator it=superKeys.begin(); it!=superKeys.end(); ++it) {
		attrOut << "," << (*it);
	}
	attrOut << endl;
	for (int i=0; i<collection.size(); i++) {
		if (collection[i].bDraw) {
			//Save patch and write attributes to file
			attrOut << collection[i].org.getFname();
			for (set<string>::iterator it=superKeys.begin(); it!=superKeys.end(); ++it) {
				attrOut << "," << collection[i].org.getAttribute(*it);
			}
			attrOut << endl;
			string outStr = path + "/" + collection[i].org.getFname();
			//ofLog(OF_LOG_VERBOSE,"Writing image to: %s", outStr.c_str());
			ofSaveImage(collection[i].org.texData.getPixelsRef(), outStr ,OF_IMAGE_QUALITY_BEST);
		}
	}
	attrOut.close();
}

void OrganismCollection::changeSphereSize(float delta) {

	sphereSize += delta;
	if (sphereSize < 1) {
		sphereSize = 1;
	}
	if (sphereSize > 100) {
		sphereSize = 100;
	}
}

void OrganismCollection::toggleDrawAxes() {
	bDrawAxes = !bDrawAxes;
}