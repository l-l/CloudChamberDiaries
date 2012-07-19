#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ofxTimeline.h"


class testApp : public ofBaseApp {
public:
	void setup();	
	void update();
	void draw();
	void particleDraw();
	void keyPressed(int key);

	int numParticles;
	float particleRadius;
	bool useShader;
	bool showFbo;
	
	bool enableAllConnecter;
	bool enableTrails;
	
	float curRotation;

	ofFbo rgbaFbo;			// fbo with alpha
	unsigned char * pixels; // the actual pixels 
	ofImage grabbed;		// img for saving
	ofEasyCam cam;			// add mouse controls for camera movement
	
	ofShader billboardShader;
	ofShader pointSpriteShader;
	ofShader lineShader;

	ofImage texture;
	ofImage colorMap;		//the images used for the coloring of the particles (image shining through)
	ofImage roomBG;
	ofColor clearColor;

	bool saveFrame;
	
	// for the timeline
	ofxTimeline timeline;
	void receivedTLTrigger(ofxTLTriggerEventArgs& trigger);
	void applyTLValues();
};
