/*
 *  Organism.cpp
 *  inSituScopeVisualizer
 *
 *  Created by Ben Laxton on 11/20/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "Organism.h"

Organism::Organism() {
	fname="";
}

Organism::~Organism() {
	texData.clear();
}

void Organism::setTexture(unsigned char * data, int w, int h, int glDataType) {
	//if(texData.bAllocated()) {
	//	texData.clear();
	//}
	
	//texData.allocate(w,h,glDataType);
	//texData.loadData(data, w, h, glDataType);
	
	ofLog(OF_LOG_WARNING,"setTexture(unsigned char*,int,int,int) not implemented!");
	return;
}

void Organism::setTexture(const ofPixels &pix) {
	if(texData.bAllocated()) {
		texData.clear();
	}
	//texData.allocate(pix);
	//texData.loadData(pix);  //Need ref?
	texData.setFromPixels(pix);
	return;
}

//Note, user should check that returns ofPixels object has been 
//Allocated, false indicates that the texture is emtpy for the 
//organism object.
ofImage Organism::getImage() {
	
	return texData;
}

map<string,float> Organism::getAttributes() {
	return attributes;
}

//Returns false if Attributes already exists, value is not overwritten
bool Organism::addAttribute(string key, float val) {
	if(std::isnan(getAttribute(key))) {
		attributes[key]=val;
		return true;
	}
	else {
		return false;
	}
}

void Organism::deleteAttribute(string key, float val) {
	attributes.erase(key);
	return;
}

float Organism::getAttribute(string key) {
	float ret;
		
	try {
		ret = attributes.at(key);      // vector::at throws an out-of-range
	}
	catch (out_of_range& oor) {
		return numeric_limits<float>::quiet_NaN();
	}
	return ret;
}

void Organism::drawOrg(float x,float y,float z) {
	texData.draw(x,y,z);
}

void Organism::setFname(string in) {
	fname = in;
}

string Organism::getFname() {
	return fname;
}