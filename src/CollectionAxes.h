/*
 *  collectionAxes.h
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"
#include "ofxFatLine.h"
#include <vector>

class CollectionAxes {
public:
	CollectionAxes();
	void setXAxis(ofPoint start, ofPoint end);
	void setZAxis(ofPoint start, ofPoint end);
	void addXAxisTick(ofPoint loc, string lbl);
	void addZAxisTick(ofPoint loc, string lbl);
	void setXAxisTicks(vector<ofPoint>, vector<string>);
	void setZAxisTicks(vector<ofPoint>, vector<string>);
	void setXLabel(string);
	void setZLabel(string);
	void setXAxisLimits(float min, float max);
	void setZAxisLimits(float min, float max);
	void showXTicks(bool val);
	void showZTicks(bool val);
	void clearXAxis();
	void clearZAxis();
	void addColorBarPt(ofVec2f loc, ofFloatColor clr, double w=10);
	void setColorBar(vector<ofVec2f>, vector<ofFloatColor>);
	void setColorBar(vector<ofVec2f>, vector<ofFloatColor>, vector<double>);
	void clearColorBar();
	
	void draw();
	
private:
	ofPoint xAxisStart;
	ofPoint xAxisEnd;
	ofPoint zAxisStart;
	ofPoint zAxisEnd;
	vector<ofPoint> xAxisTicks;
	vector<ofPoint> zAxisTicks;
	vector<string> xAxisTickLbls;
	vector<string> zAxisTickLbls;
	bool bDrawX;
	bool bDrawZ;
	string xLabel;
	string zLabel;
	float xMin;
	float xMax;
	float zMin;
	float zMax;
	bool bShowXTicks;
	bool bShowZTicks;
	
	float lineWidth;
	float tickSpacing;
	
	vector<ofVec2f> colorBarPts;
	vector<ofFloatColor> colorBarClrs;
	vector<double> colorBarWidths;
	bool bDrawClrBar;
	ofxFatLineOptions opt;
};