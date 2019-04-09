#include "ofApp.h"

#define LINE_SIZE 50
#define ROW_SIZE 250
#define FRAMERATE 48

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetFrameRate(FRAMERATE);
	float width = ofGetWidth();
	float height = ofGetHeight();
	valueIncrementer = (ROW_SIZE/LINE_SIZE) * (ROW_SIZE/LINE_SIZE);
	// setup the rtl sdr dongle
	int rowsColsVal = ROW_SIZE;
	cout << "HOW BIG IS OUR MESH? " << rowsColsVal << endl;
	// create the mesh
	for (int c = 0; c<rowsColsVal; c++){
	    for (int r = 0; r<rowsColsVal; r++){

	        glm::vec3 pos;      // grid centered at 0,0,0

	        float halfWidth     = width * 0.5;
	        float halfHeight    = height * 0.5;

	        pos.x = ofMap(r, 0, rowsColsVal-1, -halfWidth, halfWidth);
	        pos.y = ofMap(c, 0, rowsColsVal-1, halfHeight, -halfHeight);    // Y+ is up in 3D!
	        pos.z = 0;    // add depth later

	        // add the point to the mesh
	        mesh.addVertex(pos);

	        // add a color for the point
	        mesh.addColor(ofColor());

	        if (r > 0 && c > 0) {
	            int index = r * rowsColsVal + c;
	            // triangle 1
	            mesh.addIndex(index);               // A    - this pt
	            mesh.addIndex(index - 1);           // B    - + col
	            mesh.addIndex(index - rowsColsVal);        // C    - + row

	            // triangle 2
	            mesh.addIndex(index - 1);           // B
	            mesh.addIndex(index - 1 - rowsColsVal);    // D
	            mesh.addIndex(index - rowsColsVal);        // C
	        }
	    }
	}
	// Load shaders
	// shader.load("shader.vert", "shader.frag");
}

//--------------------------------------------------------------
void ofApp::update(){
	// Create LINE_SIZE random points
	if (ofGetFrameNum() % LINE_SIZE == 0) {
		for (int i = 0; i < LINE_SIZE; i++) {
			float ran = ofRandom(-280.0f, 280.0f);
			ekgLines.push_back(ran);
			ekgLinesSaved.push_back(ran);
		}
	} else {
		for (int i = 0; i < LINE_SIZE; i++) {
			ekgLines.push_back(0.0);
			ekgLinesSaved.push_back(0.0);
		}
	}
	// Delete from the front of the vector if we're
	// larger than the mesh
	if (ekgLines.size() > LINE_SIZE * LINE_SIZE) {
		for (int i = 0; i < LINE_SIZE; i++) {
			ekgLines.erase(ekgLines.begin() + i);
			ekgLinesSaved.erase(ekgLinesSaved.begin() + i);
		}
	}
	// Give em all some noise
	for (int i = 0; i < ekgLines.size(); i++) {
		float signedNoise = ofSignedNoise(i, ofGetFrameNum() * 0.05) * 5.0;
		ekgLines[i] += signedNoise;
		ekgLinesSaved[i] += signedNoise;
	}

	for (int i = 0; i < mesh.getVertices().size(); i++) {
		if (i % valueIncrementer == 0 && i/valueIncrementer < ekgLines.size()) {
			int row = (int)i/ROW_SIZE;
			float lerpValue = ofMap(abs((float)row - (float)ROW_SIZE/2.0), 0.0, (float)ROW_SIZE/2.0, 0.0, 1.0);
			ekgLines[i/valueIncrementer] = ofLerp(ekgLinesSaved[i/valueIncrementer], 0.0, lerpValue);
		}
	}

	updateZValue();
	updateColors();
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Why is this shader not fucking working
	cam.begin();
    ofEnableDepthTest();
    //shader.begin();
    //shader.setUniform1f("u_time", ofGetElapsedTimef());
    mesh.draw();
    //shader.end();
    ofDisableDepthTest();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::updateZValue(){
	for (int i = 0; i < mesh.getVertices().size(); i++) {
		glm::vec3& vertex = mesh.getVertices()[i];
		vertex.z = 0.0;
		if (i % valueIncrementer == 0 && i/valueIncrementer < ekgLines.size()) {
			vertex.z = ekgLines[i/valueIncrementer];
		} else {
			if (ceil(i/(float)valueIncrementer) <= ekgLines.size()) {
				float easedValue = easeInOutQuad((i % valueIncrementer)/(float)valueIncrementer);
				vertex.z = ofMap(easedValue, 0.0, 1.0, ekgLines[floor(i/(float)valueIncrementer)], ekgLines[ceil(i/(float)valueIncrementer)]);
			}
		}
		// float noise =
  //       ofSignedNoise ( vertex.x * 0.1,    // x pos
  //                       vertex.y * 0.1,    // y pos
  //                       ofGetElapsedTimef() * 0.5    // time (z) to animate
  //                      );
  //       vertex.z += noise * 10.0;
	}
}

// --------------------------------
void ofApp::updateColors(){

    // map colors based on vertex z / depth

    for (int i=0; i<mesh.getVertices().size(); i++){

        // 1 color per vertex
        glm::vec3& vertex = mesh.getVertices()[i];

        // get depth as percent of noise range
        float depthPercent = ofMap(vertex.z, -noiseAmp, noiseAmp, 0, 1, true);    // map 0-1

        // lerp color
        ofColor color = colorFar.getLerped( colorNear, depthPercent );

        mesh.setColor(i, color);        // set mesh color
    }

}

// --------------------------------
// t = value between [0, 1] to add ease to
// https://github.com/jesusgollonet/ofpennereasing/blob/master/PennerEasing/Quad.cpp
float ofApp::easeInOutQuad(float t) {
	if (t < 0.5) {
		return (2.0 * t * t);
	} else {
		return (-1.0 + (4.0 - 2.0 * t) * t);
	}

}
