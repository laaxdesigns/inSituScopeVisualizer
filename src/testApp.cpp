#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetupScreenPerspective(1024, 768, OF_ORIENTATION_DEFAULT, true, 100, .1, 50000);
	ofSetFrameRate(30);
	ofDisableArbTex();
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	newFett.loadFont("GUI/NewMedia Fett.ttf", 10);
	sortKeyX.clear();
	sortKeyZ.clear();
	bMouseOver=false;
	
	
	imgDir.clear();
	attrPath.clear();
	
	//bSOMguiInit=false;
	//bSOMguiInit=false;
	SOMgui=NULL;
	loadGui=NULL;
	initLoadGui();
	activeGUI = "loadGui";
	coll = NULL;
	
	
	///////////////////////////////////////////////////
	// setup the camera
	///////////////////////////////////////////////////
	camDist  = 2000;
	camera.setDistance(camDist);
	//camera.setTarget(coll);
	camera.disableMouseInput();
}

//--------------------------------------------------------------
void testApp::update(){
	if (bMouseOver) {
		map<int,ofPoint> orgs = coll->getPatchLocs();
		map<int,ofPoint>::iterator it;
		
		float nearestDistance = 0;
		//ofVec2f nearestVertex;
		//int nearestID;
		ofVec2f mouse(mouseX, mouseY);
		for ( it=orgs.begin() ; it != orgs.end(); it++ ) {
			ofVec3f pPos,w2s;
			pPos.set((*it).second.x,(*it).second.y,(*it).second.z);
			w2s = camera.worldToScreen(pPos);
			float d = w2s.distance(mouse);
			if(it==orgs.begin() || d<nearestDistance) {
				nearestDistance=d;
				nearestVertex=w2s;
				nearestID = (*it).first;
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	if (activeGUI=="loadGui") {
		ofPushStyle();
		ofSetColor(10, 75, 200);
		string tmp = "ROI Directory = " + imgDir;
		newFett.drawString(tmp, 50, 50);
		tmp = "Attributes File = " + attrPath;
		newFett.drawString(tmp, 50, 75);
		ofPopStyle();
	}
	else if(activeGUI=="collectionGui") {
		 
		//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		glEnable(GL_DEPTH_TEST);
		camera.begin();
		coll->draw();
		camera.end();
		//ofDisableAlphaBlending();

		//glDepthMask(GL_TRUE);
		if(bMouseOver) {
			//Draw line to nearest Patch
			ofPushStyle();
			ofSetColor(ofColor::red);
			ofLine(nearestVertex,ofVec2f(mouseX,mouseY));
			ofSetColor(ofColor::yellow);
			ofCircle(nearestVertex,4);
			ofPopStyle();
			
			//Display patch information in the upper right corner of screen
			Organism nearestOrg = coll->getOrganismByID(nearestID);
			
			if(nearestPatch.bAllocated()) {
				nearestPatch.clear();
			}
			nearestPatch = nearestOrg.getImage();
			if(nearestPatch.bAllocated()) {
				float tmpX = ofGetWindowWidth()-(nearestPatch.getWidth()*2+120);
				nearestPatch.draw(tmpX, 10,nearestPatch.getWidth()*2,nearestPatch.getHeight()*2);
				float offset = nearestPatch.getHeight()*2+20;
				map<string,float> attr = nearestOrg.getAttributes();
				map<string,float>::iterator it;
				for (it=attr.begin(); it!=attr.end(); it++) {
					ofPushStyle();
					ofSetColor(255, 100, 0);
					ofDrawBitmapString((*it).first + ":" + ofToString((*it).second), tmpX, offset);
					offset += 10;
					ofPopStyle();
				}
			}
		}
	}
	else {
		string tmp = "Active Gui " + activeGUI + " is an error condition!";
		newFett.drawString(tmp, 300, 50);
	}

	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == OF_KEY_UP) {
		camDist -= 20;
		camera.setDistance(camDist);
	}
	else if(key == OF_KEY_DOWN) {
		camDist += 20;
		camera.setDistance(camDist);
	}
	else if(key == 's') {
		coll->setDrawType(OrganismCollection::SPHERE);
	}
	else if(key == 'p') {
		coll->setDrawType(OrganismCollection::PATCH);
	}
	else if(key == 'r') {
		coll->randomizePositions();
	}
	else if(key == 'a') {
		coll->setAnimate(!coll->getAnimate());
	}
	else if(key =='l') {
		if (SOMgui->isVisible()) {
			camera.enableMouseInput();
		}
		else {
			camera.disableMouseInput();
		}
		SOMgui->toggleVisible();
	}
	else if(key == 'f') {
		ofToggleFullscreen();
		SOMgui->setScrollArea(0, 0, ofGetScreenWidth(), ofGetScreenHeight());
	}
	else if(key == 'c') {
		coll->printLocs();
	}
	else if(key == 'm') {
		bMouseOver = !bMouseOver;
	}
	else if(key == '-') {
		coll->changeSphereSize(-0.5);
	}
	else if(key == '+') {
		coll->changeSphereSize(0.5);
	}
	else if(key == 'i') {
		coll->toggleDrawAxes();
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(bMouseOver && button==2) {
		coll->setOrganismToFalse(nearestID);
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	if (activeGUI=="collecectionGui") {
		SOMgui->getRect()->setWidth(ofGetWindowWidth());
	}
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void testApp::exit() {
	clearLoadGui();
	clearCollectionGui();
	if (coll != NULL) {
		delete coll;
	}
}

//--------------------------------------------------------------
void testApp::SOMguiEvent(ofxUIEventArgs &e) {
	string name = e.widget->getName();
	ofLog(OF_LOG_VERBOSE,"Got event from %s.",name.c_str());
	ofLog(OF_LOG_VERBOSE,"Parent = %s.",e.widget->getParent()->getName().c_str());

	
	if (name=="SOM Attributes") {
		ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
		if (!ddlist->isOpen()) {
			vector<ofxUIWidget *> &selected = ddlist->getSelected();
			for (int i=0; i<keys.size(); i++) {
				ofLog(OF_LOG_VERBOSE,"Setting all keys to false...");
				coll->setKeyUse(keys[i], false);
			}
			for(int i = 0; i < selected.size(); i++)
			{
				ofLog(OF_LOG_VERBOSE,"Setting %s to true.",selected[i]->getName().c_str());
				coll->setKeyUse(selected[i]->getName(), true);
			}	
		}
	}
	else if(name=="Sort Key - X") {
		ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
		if (!ddlist->isOpen()) {
			vector<ofxUIWidget *> &selected = ddlist->getSelected();
			if (selected.size()==1) {
				sortKeyX = selected[0]->getName();
				coll->updatePosBySortedKeys(sortKeyX,sortKeyZ);
			}
			else {
				sortKeyX.clear();
			}

		}
	}
	else if(name=="Sort Key - Z") {
		ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
		if (!ddlist->isOpen()) {
			vector<ofxUIWidget *> &selected = ddlist->getSelected();
			if (selected.size()==1) {
				sortKeyZ = selected[0]->getName();
				coll->updatePosBySortedKeys(sortKeyX,sortKeyZ);
			}
			else {
				sortKeyZ.clear();
			}
			
		}
	}
	else if(name=="Color Key") {
		ofxUIDropDownList *ddlist = (ofxUIDropDownList *) e.widget;
		if (!ddlist->isOpen()) {
			vector<ofxUIWidget *> &selected = ddlist->getSelected();
			if (selected.size()==1) {
				string colorKey = selected[0]->getName();
				coll->updateColorByKey(colorKey);
			}
		}
	}
	else if(name=="Learn SOM") {
		ofxUIButton *btn = (ofxUIButton *) e.widget;
		if (btn->getValue()) {
			coll->learnSOM();
		}
	}
	else if(name=="RESET") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			activeGUI = "loadGui";
			initLoadGui();
		}
	}
	else if(name == "ShowAll") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			coll->showAll();
		}
	}
	else if(name == "Save") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			ofFileDialogResult openFileResult= ofSystemLoadDialog("Choose a directory.",true);
			if (openFileResult.bSuccess) {
				//ofLog(OF_LOG_VERBOSE,"%s",openFileResult.getPath().c_str());
				coll->saveVisible(openFileResult.getPath());
			}
		}
	}
	else {
		for (int i=0; i<keys.size(); i++) {
			if (name==keys[i]) {
				if (e.widget->getParent()->getName()=="Sort Key - X") {
					//Do nothing event is handled when Drop-Down is closed
				}
				else if(e.widget->getParent()->getName()=="Sort Key - Z") {
					//Do nothing event is handled when Drop-Down is closed
				}
				else if(e.widget->getParent()->getName()=="SOM Attributes") {
					//Do nothing event is handled when Drop-Down is closed
				}
				else if(e.widget->getParent()->getName()=="Color Key") {
					//Do Nothing event is handled when Drop-Down is closed
				}
				else {
					ofxUIRangeSlider *sldr = (ofxUIRangeSlider *) e.widget;
					coll->setKeyThresh(name, sldr->getScaledValueLow(), sldr->getScaledValueHigh());
				}
			}
		}
	}

}

void testApp::loadGuiEvent(ofxUIEventArgs &e) {
	string name = e.widget->getName();
	ofLog(OF_LOG_VERBOSE,"Got event from %s.",name.c_str());
	ofLog(OF_LOG_VERBOSE,"Parent = %s.",e.widget->getParent()->getName().c_str());
	
	if (name == "LOAD COLLECTION") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			if (!imgDir.empty() && !attrPath.empty()) {
				if(initCollection() != -1) {
					activeGUI = "collectionGui";
					initCollectionGui();
				}
			}
		}
	}
	else if(name == "SET ROI DIRECTORY") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			ofFileDialogResult openFileResult= ofSystemLoadDialog("Choose the directory containing the ROI images.",true);
			if (openFileResult.bSuccess) {
				imgDir = openFileResult.getPath();
			}
		}
	}
	else if(name == "SET FEATURES FILE") {
		ofxUILabelButton *btn = (ofxUILabelButton *) e.widget;
		if (btn->getValue()) {
			ofFileDialogResult openFileResult= ofSystemLoadDialog("Choose the csv-file containing ROI attributes values.");
			if (openFileResult.bSuccess) {
				attrPath = openFileResult.getPath();
				ofFile file(attrPath);
				if(!file.exists()){
					ofLogError("The file " + attrPath + " is missing");
				}
				string fileExtension = ofToUpper(file.getExtension());
				if (fileExtension == "TXT" || fileExtension == "CSV") {
					//ok
				}
				else {
					ofLog(OF_LOG_VERBOSE,"Files of type %s are not supported.",fileExtension.c_str());
					attrPath.clear();
				}

			}
		}
	}
}

int testApp::initCollection() {
	////////////////////////////////////////////////////
	//collection initialization stuff
	////////////////////////////////////////////////////
	if (coll != NULL) {
		delete coll;
		coll = NULL;
	}
	coll = new OrganismCollection;
	ofFile file(attrPath);
	if(!file.exists()){
		ofLogError("The file " + attrPath + " is missing");
	}
	ofBuffer attrBuff(file);
	int numAdded = coll->initByHeaderedAttrFile(attrBuff, imgDir);
	if (numAdded != -1) {
		keys = coll->getKeySuperSet();
		ofLog(OF_LOG_VERBOSE,"KEYS SUPERSET");
		for (int i=0; i<keys.size(); i++) {
			ofLog(OF_LOG_VERBOSE,"%s max=%.04f \t min=%.04f \t use=%s",
				  keys[i].c_str(),
				  coll->getKeyMax(keys[i]),
				  coll->getKeyMin(keys[i]),
				  (coll->getKeyUse(keys[i]))?"true":"false");
			coll->setKeyUse(keys[i], false);
		}
		
		ofLog(OF_LOG_VERBOSE,"Added %d organisms\n",numAdded);
	}
	else {
		delete coll;
		coll = NULL;
	}


	return numAdded;
	
}

void testApp::initCollectionGui() {
	/////////////////////////////////////////////////////
	//Initialize the gui elements
	/////////////////////////////////////////////////////
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float dim = 15;
	float length = 240-xInit;
	SOMgui = new ofxUIScrollableCanvas(0,0,length,ofGetHeight());
	//SOMgui->setScrollAreaToScreenHeight();
	SOMgui->setScrollArea(0, 0, length, ofGetHeight());
	SOMgui->setScrollableDirections(false, true);
	
	//Range Sliders for keys
	for (int i=0; i<keys.size(); i++) {
		float max = coll->getKeyMax(keys[i]);
		float min = coll->getKeyMin(keys[i]);
		SOMgui->addRangeSlider(keys[i], min,max,min,max, length-xInit,dim);
	}
	//DynamicDropDown Toggle 
	SOMgui->addWidgetEastOf(new ofxUIButton("Learn SOM",false,dim,dim), keys[0]);
	ofxUIDropDownList *ddl;
	ddl = new ofxUIDropDownList("SOM Attributes",keys,length-xInit);
	ddl->setAllowMultiple(true);
	SOMgui->addWidgetSouthOf(ddl, "Learn SOM");
	//Toggle for sorting by key
	ofxUIDropDownList *ddl2;
	ddl2 = new ofxUIDropDownList("Sort Key - X",keys,length-xInit);
	ddl2->setAllowMultiple(false);
	SOMgui->addWidgetEastOf(ddl2, "SOM Attributes"); 
	ofxUIDropDownList *ddl3;
	ddl3 = new ofxUIDropDownList("Sort Key - Z",keys,length-xInit);
	ddl3->setAllowMultiple(false);
	SOMgui->addWidgetEastOf(ddl3, "Sort Key - X"); 
	ofxUIDropDownList *ddl4;
	ddl4 = new ofxUIDropDownList("Color Key",keys,length-xInit);
	ddl4->setAllowMultiple(false);
	SOMgui->addWidgetEastOf(ddl4, "Sort Key - Z");
	SOMgui->addWidgetEastOf(new ofxUILabelButton("RESET",false,150,dim), "Learn SOM");
	SOMgui->addWidgetEastOf(new ofxUILabelButton("ShowAll",false,150,dim),  "RESET");
	SOMgui->addWidgetEastOf(new ofxUILabelButton("Save",false,150,dim),  "ShowAll");
	
	SOMgui->setTheme(OFX_UI_THEME_HIPSTER);
	SOMgui->autoSizeToFitWidgets();
	SOMgui->getRect()->setWidth(ofGetWindowWidth());
	
	ofAddListener(SOMgui->newGUIEvent, this, &testApp::SOMguiEvent);
	if (loadGui) {
		loadGui->disable();
	}
}

void testApp::initLoadGui() {
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
	float dim = 15;
	float length = 240-xInit;
	loadGui = new ofxUICanvas();
	loadGui->addWidget(new ofxUILabelButton("LOAD COLLECTION", false, 220, dim, ofGetWindowWidth()/2, ofGetWindowHeight()/2, OFX_UI_FONT_MEDIUM));
	loadGui->addWidgetEastOf(new ofxUILabelButton("SET ROI DIRECTORY", false, 220, dim), "LOAD COLLECTION");
	loadGui->addWidgetWestOf(new ofxUILabelButton("SET FEATURES FILE", false, 220, dim), "LOAD COLLECTION");
	loadGui->setTheme(OFX_UI_THEME_HIPSTER);
	
	ofAddListener(loadGui->newGUIEvent, this, &testApp::loadGuiEvent);
	if (SOMgui) {
		SOMgui->disable();
	}
}

void testApp::clearCollectionGui() {
	if (SOMgui!=NULL) {
		delete SOMgui;
		SOMgui = NULL;
	}
}

void testApp::clearLoadGui() {
	if (loadGui!=NULL) {
		delete loadGui;
		loadGui = NULL;
	}
}