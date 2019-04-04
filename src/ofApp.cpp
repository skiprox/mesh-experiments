#include "ofApp.h"

#define LINE_SIZE 150

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	float width = ofGetWidth();
	float height = ofGetHeight();
	// setup the rtl sdr dongle
	int rowsColsVal = LINE_SIZE;
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
	// Create 100 random points
	if (ofGetFrameNum() % LINE_SIZE == 0) {
		for (int i = 0; i < LINE_SIZE; i++) {
			ekgLines.push_back(ofRandom(-80.0f, 80.0f));
			ekgLinesSaved.push_back(ofRandom(-80.0f, 80.0f));
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

	for (int i = 0; i < ekgLines.size(); i++) {
		int row = floor (i/LINE_SIZE);
		// cout << row << endl;
		float lerpValue = ofMap((float) row, 0.0, (float) LINE_SIZE, 1.0, 0.0);
		//float lerpValue = 1.0/(row + 1.0);
		// cout << lerpValue << endl;
		ekgLines[i] = ofLerp(ekgLinesSaved[i], 0.0, lerpValue);
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
		vertex.z = ekgLines[i];
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