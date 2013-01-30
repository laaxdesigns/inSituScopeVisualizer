#pragma once

#include "ofMain.h"

#include "ofxUI.h"

#include "OrganismCollection.h"


class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	void SOMguiEvent(ofxUIEventArgs &e);
	int initCollection();
	void initCollectionGui();
	void initLoadGui();
	void loadGuiEvent(ofxUIEventArgs &e);
	void clearCollectionGui();
	void clearLoadGui();
	
private:
	OrganismCollection *coll;
	vector<string> keys;
	ofEasyCam camera;
	
	float camDist;
	string curKey;
	
	ofxUIScrollableCanvas *SOMgui;
	ofxUICanvas *loadGui;
	string activeGUI;
	
	string sortKeyX;
	string sortKeyZ;
	
	bool bMouseOver;
	int nearestID;
	ofVec2f nearestVertex;
	ofImage nearestPatch;
	
	ofTrueTypeFont newFett;
	
	string imgDir;
	string attrPath;
};
