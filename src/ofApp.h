#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void updateZValue();
		void updateColors();

		float easeInOutQuad(float t);

		// mesh stuff
		ofEasyCam cam;
		ofMesh mesh;
		ofColor colorNear = ofColor(101, 114, 235);
		ofColor colorFar = ofColor(203, 255, 181);
		float noiseAmp = 40.f;
		vector<float> ekgLines;
		vector<float> ekgLinesSaved;
		int valueIncrementer;

		// Shaders
		ofShader shader;
		
};
