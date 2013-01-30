/*
 *  collectionAxes.cpp
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 1/24/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */

#include "CollectionAxes.h"

CollectionAxes::CollectionAxes() {
	xAxisStart.set(0,0,0);
	xAxisEnd.set(0,0,0);
	zAxisStart.set(0,0,0);
	zAxisEnd.set(0,0,0);
	xAxisTicks.clear();
	zAxisTicks.clear();
	xAxisTickLbls.clear();
	zAxisTickLbls.clear();
	bDrawX = false;
	bDrawZ = false;
	xLabel.clear();
	zLabel.clear();
	xMin = 0;
	xMax = 0;
	zMin = 0;
	zMax = 0;
	bShowXTicks = false;
	bShowZTicks = false;
	
	lineWidth = 10.0;
	tickSpacing = 10.0;
	
	colorBarPts.clear();
	colorBarClrs.clear();
	colorBarWidths.clear();
	bDrawClrBar = false;	
	opt.feather    = true;
	opt.feathering = 1.0;
	opt.no_feather_at_cap = false;
	opt.no_feather_at_core = false;
	opt.cap   = OFX_FATLINE_CAP_ROUND;
    opt.joint = OFX_FATLINE_JOINT_ROUND;
}

void CollectionAxes::setXAxis(ofPoint s, ofPoint e) {
	xAxisStart = s;
	xAxisEnd = e;
	bDrawX = true;
}

void CollectionAxes::setZAxis(ofPoint s, ofPoint e) {
	zAxisStart = s;
	zAxisEnd = e;
	bDrawZ = true;
}

void CollectionAxes::setXLabel(string lbl) {
	xLabel = lbl;
}

void CollectionAxes::setZLabel(string lbl) {
	zLabel = lbl;
}

void CollectionAxes::addXAxisTick(ofPoint loc, string lbl) {
	xAxisTicks.push_back(loc);
	xAxisTickLbls.push_back(lbl);
}

void CollectionAxes::addZAxisTick(ofPoint loc, string lbl) {
	zAxisTicks.push_back(loc);
	zAxisTickLbls.push_back(lbl);
}

void CollectionAxes::setXAxisTicks(vector<ofPoint> tics, vector<string> lbls) {
	xAxisTicks.clear();
	xAxisTickLbls.clear();
	xAxisTicks = tics;
	xAxisTickLbls = lbls;
}

void CollectionAxes::setZAxisTicks(vector<ofPoint> tics, vector<string> lbls) {
	zAxisTicks.clear();
	zAxisTickLbls.clear();
	zAxisTicks = tics;
	zAxisTickLbls = lbls;
}

void CollectionAxes::clearXAxis() {
	xAxisStart.set(0,0,0);
	xAxisEnd.set(0,0,0);
	xAxisTicks.clear();
	xAxisTickLbls.clear();
	xLabel.clear();
	xMin = 0;
	xMax = 0;
	bShowXTicks = false;
	bDrawX = false;
}

void CollectionAxes::clearZAxis() {
	zAxisStart.set(0,0,0);
	zAxisEnd.set(0,0,0);
	zAxisTicks.clear();
	zAxisTickLbls.clear();
	zLabel.clear();
	zMin = 0;
	zMax = 0;
	bShowZTicks = false;
	bDrawZ = false;
}

void CollectionAxes::setXAxisLimits(float mn, float mx) {
	xMin = mn;
	xMax = mx;
}

void CollectionAxes::setZAxisLimits(float mn, float mx) {
	zMin = mn;
	zMax = mx;
}

void CollectionAxes::showXTicks(bool val) {
	bShowXTicks = val;
}
void CollectionAxes::showZTicks(bool val) {
	bShowZTicks = val;
}

void CollectionAxes::addColorBarPt(ofVec2f loc, ofFloatColor clr, double w) {
	colorBarPts.push_back(loc);
	colorBarClrs.push_back(clr);
	colorBarWidths.push_back(w);
	bDrawClrBar = true;
}

void CollectionAxes::setColorBar(vector<ofVec2f> locs, vector<ofFloatColor> clrs) {
	colorBarPts.clear();
	colorBarClrs.clear();
	colorBarWidths.clear();
	colorBarPts = locs;
	colorBarClrs = clrs;
	colorBarWidths.assign(colorBarPts.size(), 10);
	bDrawClrBar = true;
}

void CollectionAxes::setColorBar(vector<ofVec2f> locs, vector<ofFloatColor> clrs, vector<double> widths) {
	colorBarPts.clear();
	colorBarClrs.clear();
	colorBarWidths.clear();
	colorBarPts = locs;
	colorBarClrs = clrs;
	colorBarWidths = widths;
	bDrawClrBar = true;
}

void CollectionAxes::clearColorBar() {
	colorBarPts.clear();
	colorBarClrs.clear();
	colorBarWidths.clear();
	bDrawClrBar = false;
}

void CollectionAxes::draw() {
	if(bDrawX) {
		ofPushStyle();
		ofSetLineWidth(lineWidth);
		ofSetColor(0, 255, 255);
		ofLine(xAxisStart, xAxisEnd);
		if (!xLabel.empty()) {
			ofSetColor(0, 255, 0);
			ofPoint tmp;
			tmp = xAxisStart;
			tmp.x += (xAxisEnd.x-xAxisStart.x)/2;
			tmp.y += 10;
			ofDrawBitmapString(ofToUpper(xLabel), tmp);
		}
		if(bShowXTicks) {
			ofSetColor(255, 69, 0);
			float tickOffset = (xAxisEnd.x-xAxisStart.x)/tickSpacing;
			float curTickLoc = xAxisStart.x;
			float tickDelta = (xMax-xMin)/tickSpacing;
			float tickStart = xMin;
			while(tickStart < xMax) {
				ofPoint tmp;
				tmp = xAxisStart;
				tmp.x = curTickLoc;
				ofDrawBitmapString(ofToString(tickStart), tmp);
				tickStart += tickDelta;
				curTickLoc += tickOffset;
			}
		}
		//Later may support custom tick labels
		/*
		for (int i=0; i<xAxisTicks.size(); i++) {
			ofDrawBitmapString(xAxisTickLbls[i], xAxisTicks[i]);
		}
		*/
		ofPopStyle();
	}
	
	if(bDrawZ) {
		ofPushStyle();
		ofSetLineWidth(lineWidth);
		ofSetColor(0, 255, 255);
		ofLine(zAxisStart, zAxisEnd);
		if (!zLabel.empty()) {
			ofSetColor(0, 255, 0);
			ofPoint tmp;
			tmp = zAxisStart;
			tmp.z += (zAxisEnd.z-zAxisStart.z)/2;
			tmp.y += 10;
			ofDrawBitmapString(ofToUpper(zLabel), tmp);
		}
		if(bShowZTicks) {
			ofSetColor(255, 69, 0);
			float tickOffset = (zAxisEnd.z-zAxisStart.z)/tickSpacing;
			float curTickLoc = xAxisStart.z;
			float tickDelta = (zMax-zMin)/tickSpacing;
			float tickStart = zMin;
			while(tickStart < zMax) {
				ofPoint tmp;
				tmp = zAxisStart;
				tmp.z = curTickLoc;
				ofDrawBitmapString(ofToString(tickStart), tmp);
				tickStart += tickDelta;
				curTickLoc += tickOffset;
			}
		}
		//Later may support custom tick labels
		/*
		for (int i=0; i<zAxisTicks.size(); i++) {
			ofDrawBitmapString(zAxisTickLbls[i], zAxisTicks[i]);
		}
		 */
		ofPopStyle();
	}
	
	if(bDrawClrBar) {
		ofxFatLine(&colorBarPts[0], &colorBarClrs[0], &colorBarWidths[0], colorBarPts.size(), 0);
	}
	
}