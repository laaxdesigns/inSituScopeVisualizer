/*
 *  Organism.h
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 11/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#include "ofMain.h"
#include <map>
#include <string>
#include <stdexcept>

//using namespace std;

class Organism {

public:
	Organism();
	~Organism();
	void setTexture(unsigned char * data, int w, int h, int glDataType);
	void setTexture(const ofPixels &pix);
	ofImage getImage();
	map<string,float> getAttributes();
	bool addAttribute(string key, float val);
	void deleteAttribute(string key, float val);
	float getAttribute(string key);
	void drawOrg(float x,float y,float z);
	void setFname(string);
	string getFname();

	ofImage texData;
protected:
	map<string, float> attributes;
	string fname;
	

};