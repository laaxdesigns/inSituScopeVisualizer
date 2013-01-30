/*
 *  OrganismCollection.h
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 11/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Organism.h"
#include "ofxSelfOrganizingMap.h"
#include "CollectionAxes.h"

class OrganismCollection {
	struct patch {
		Organism org;
		ofPoint position;
		ofColor color;
		bool bDraw;
		vector<ofPoint> path;
		vector<ofVec3f> velocity;
		int pid;
	};
	
public:
	enum DrawType {
		SPHERE,
		PATCH
	};
	
	OrganismCollection();
	int initByHeaderedAttrFile(ofBuffer attr, string imgLoc);
	void draw();
	vector<string> addOrganism(Organism oIn);
	int size();
	vector<string> getKeySuperSet();
	void setDrawType(DrawType);
	void randomizePositions();
	void setAnimate(bool);
	bool getAnimate();
	float getKeyMax(string);
	float getKeyMin(string);
	bool getKeyUse(string);
	void setKeyUse(string,bool);
	void learnSOM();
	void setKeyThresh(string key, float min, float max);
	void printLocs();
	void updatePosBySortedKeys(string keyX,string keyZ);
	map<int,ofPoint> getPatchLocs();
	Organism getOrganismByID(int idIN);
	void updateColorByKey(string);
	void setOrganismToFalse(int idIN);
	void showAll();
	void saveVisible(string path);
	void changeSphereSize(float delta);
	void toggleDrawAxes();
	
protected:
	void update();
	void setupSOM();
	void runSOMIter();
	void updatePosBySOMMap();
	vector<double> getInstanceVec(int ind);
	void filter();
	
private:
	static int globID;
	
	vector<patch> collection;
	set<string> superKeys;
	map<string,float> maxKeyVals;
	map<string,float> minKeyVals;
	map<string,float> maxKeyThresh;
	map<string,float> minKeyThresh;
	map<string,bool> useKeys;
	DrawType drawT;
	bool bUpdatePos;
	bool bAnimate;
	ofxSelfOrganizingMap som;
	bool bReInitSOM;
	int numSOMIters;
	int SOMiCount;
	int SOM_dims;
	bool bLearnSOM;
	int mapSX;
	int mapSY;
	int mapSZ;
	float sphereSize;
	
	CollectionAxes axes;
	bool bDrawAxes;
};