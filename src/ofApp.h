#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void updateZValue();
		void updateColors();

		// mesh stuff
		ofEasyCam cam;
		ofMesh mesh;
		ofColor colorNear = ofColor::fuchsia;
		ofColor colorFar = ofColor::navy;
		float noiseAmp = 40.f;
		vector<float> ekgLines;
		vector<float> ekgLinesSaved;

		// Shaders
		ofShader shader;
		
};
