#include "testApp.h"

void testApp::setup() 
	{
		
		
	 // Timeline Setup

	timeline.setup();
	timeline.setDurationInFrames(5000);
	timeline.setLoopType(OF_LOOP_NORMAL);

	timeline.setPageName("FlockParameters");
	
	/*
	age = 0;
	globalDirection = 0;
	*/
	timeline.addKeyframes("speed", "speed.xml", ofRange(0, 5));
	timeline.addKeyframes("spread", "spread.xml", ofRange(0, 100));
	timeline.addKeyframes("viscosity", "viscosity.xml", ofRange(0, 2));
	timeline.addKeyframes("independence", "independence.xml", ofRange(0, 2));
	timeline.addKeyframes("neighborhood", "neighborhood.xml", ofRange(1, 1000));
	timeline.addKeyframes("confusion", "confusion.xml", ofRange(0, 1));
	timeline.addKeyframes("indecision", "indecision.xml", ofRange(0, 1));


	
	timeline.addPage("forceWeights",true);
	timeline.addKeyframes("weightCenteringForce", "weightCenteringForce.xml", ofRange(0, 1));
	timeline.addKeyframes("weightExternalForce", "weightExternalForce.xml", ofRange(0, 1));
	timeline.addKeyframes("weightFlockingForce", "weightFlockingForce.xml", ofRange(0, 1));
	timeline.addKeyframes("weightSphericalForce", "weightSphericalForce.xml", ofRange(0, 1));
	timeline.addKeyframes("weightFunnelForce", "weightFunnelForce.xml", ofRange(0, 1));
	timeline.addKeyframes("weightAttractionPointForce", "weightAttractionPointForce.xml", ofRange(0, 1));
	

	timeline.addTriggers("attractionModel", "attractionModel.xml");
	//timeline.getColors().loadColors("defaultColors.xml");
	ofAddListener(ofxTLEvents.trigger, this, &testApp::receivedTLTrigger);

	enableAllConnecter=false;
	enableTrails=false;
	//-----

	 ofSetLogLevel(OF_LOG_VERBOSE);

	 saveFrame=false;
	 numParticles=6000;
	 particleRadius=400;
	 useShader=true;
	 showFbo=false; // draw into an fbo, for alpha image saving
	 curRotation=0;
	 clearColor=ofColor(0,0,0, 0);


	//ofSetBackgroundAuto(true);
	 ofSetVerticalSync(true);
	 glEnable(GL_POINT_SMOOTH);
	 ofEnableAlphaBlending();
	 ofEnableBlendMode(OF_BLENDMODE_ALPHA); //ofEnableAlphaBlending();


    //some model / light stuff
    glShadeModel(GL_SMOOTH);
//    light.enable();
 //   ofEnableSeparateSpecularLight();


	 glEnable(GL_DEPTH_TEST);
	 glEnable(GL_BLEND);  
      // set blend mode  
     //glBlendFunc(GL_SRC_ALPHA,GL_ONE);  
	 	
	 
	Particle::setup(numParticles, particleRadius);  // num, radius	

	pixels = new unsigned char[1920*1080*4];  
	grabbed.allocate(1920, 1080, OF_IMAGE_COLOR_ALPHA);
	
	billboardShader.load("Billboard");
	lineShader.load("AllConnecterShader");
	//pointSpriteShader.load("PointSpriteMask");
	colorMap.loadImage("img2.png");
		
	roomBG.loadImage("room.png");
	// we need to disable ARB textures in order to use normalized texcoords
	ofDisableArbTex();
	texture.loadImage("dot.png");
	

	rgbaFbo.allocate(1920, 1080, GL_RGBA);
	rgbaFbo.begin();
	//ofClear(150,150,150, 0);
    rgbaFbo.end();
	
}

void testApp::update() 
	{
	curRotation+=(  (1080-mouseY)/4.  -curRotation)/40.;
	
	

	if(ofGetMousePressed()) {
		//Particle::addForce(ofVec3f(mouseX - ofGetWidth() / 2, mouseY - ofGetHeight() / 2, ofRandom(-particleRadius,particleRadius)), 300);
	}
	//Particle::updateAll(60 * ofGetLastFrameTime());
	Particle::updateAll(1.);
	if (showFbo) 
		{rgbaFbo.begin();
		
	//	cam.begin();
		particleDraw();
	//	cam.end();
		rgbaFbo.end();
		}
	 Particle::allConnecterMaxLength=mouseX/10.;
	 Particle::allConnecterMinLength=mouseY/10.;
}

void testApp::draw() {	

		ofClear(clearColor.r,clearColor.g,clearColor.b,clearColor.a);
  		
		
    if (showFbo) rgbaFbo.draw(0,0);
	else		 particleDraw();
	
	if (saveFrame && showFbo) // save from fbo with alpha
		{rgbaFbo.begin();  
  
			glPixelStorei(GL_PACK_ALIGNMENT, 1);  
			glReadPixels(0, 0, 1920, 1080, GL_RGBA, GL_UNSIGNED_BYTE, pixels);  
  
			grabbed.setFromPixels(pixels, 1920, 1080, OF_IMAGE_COLOR_ALPHA);  
		//	grabbed.grabScreen(0,0,1920,1080);
			grabbed.saveImage("\grab0\contfboSave"+ofToString(ofGetFrameNum())+".png");  
			printf("%s","SAAAVE");
		rgbaFbo.end();  
	//	saveFrame=false;
		}
	else if (saveFrame) // save normal
		{grabbed.grabScreen(0,0,1920,1080);
	grabbed.saveImage("grab/contGrabSave"+ofToString(ofGetFrameNum())+".png");  
		}
	timeline.draw();
	applyTLValues();

}

void testApp::particleDraw()
	{//ofDisableLighting(); 
		
	 glEnable(GL_BLEND);  
      // set blend mode  
      // glBlendFunc(GL_SRC_ALPHA,GL_ONE);  

	 ofClear(clearColor.r,clearColor.g,clearColor.b,clearColor.a);  // this has an issue, the clearcolor is visible in the antialiasing pixels
	 
	 // draw the reference image that shines through in the cloud
	 // colorMap.draw(0,1080,0,1920,-1080); 

	 // if (!showFbo) roomBG.draw(-410,-320,-400,1920*1.5,1080*1.5);
	 //glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); // ...
	 //glBlendEquation(GL_FUNC_ADD);
	 //ofSetColor(255,250,255,200);
	
		ofPushMatrix();
	    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 100); // width & height offset
		ofRotate(90,0,0,1);
	  //  ofRotate(curRotation,0,1,0);
	  //  ofRotate(50,0,0,1);
	    
		// INTERACTORS -----------------------------
		Particle::drawInteractors();

		// PARTICLES -------------------------------
		if (useShader)
			{billboardShader.begin();
			 billboardShader.setUniformTexture("tex1", colorMap, 2 );
			 ofEnablePointSprites();
		     texture.getTextureReference().bind();//texture.getTextureReference().bind();
			 glDisable(GL_DEPTH_TEST);
			}
		
		Particle::drawParticles();
		glEnable(GL_DEPTH_TEST);
		if (useShader)
			{texture.getTextureReference().unbind();//texture.getTextureReference().unbind();
			 ofDisablePointSprites();
		  	 billboardShader.end();
		 	}
		
		// CONNECTERS -------------------------------
		//Particle::drawConnecter();
		
		// TRAILS -----------------------------------
		glDisable(GL_DEPTH_TEST);
		if (enableTrails) 
			{Particle::enableTrails();
			 Particle::drawTrails();
			}
		else 
			{Particle::disableTrails(); 
			if (Particle::doTrail) Particle::drawTrails();
			}

		// ALLCONNECTER -----------------------------
		if (enableAllConnecter) 
			{	lineShader.begin();
					lineShader.setUniformTexture("tex1", colorMap, 0 );
			 		Particle::drawAllConnecter();
				lineShader.end();
			}
		
		
		//Particle::drawLines(); //the white velocity lines

		ofDisablePointSprites();
		glPointSize(10.);
		ofSetColor(255,0,0,255);
		


		glEnable(GL_DEPTH_TEST);
	ofPopMatrix();
	
	 // framrate debug
	ofSetColor(0);
	ofRect(0, 0, 100, 100);
	ofSetColor(255);
	
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
	

	}


void testApp::receivedTLTrigger(ofxTLTriggerEventArgs& trigger){
	if(trigger.triggerName == "PRISM"){
		Particle::selectAttractionMesh(1);
	}
	else if(trigger.triggerName == "DOG"){
		Particle::selectAttractionMesh(0);
	}
	else if(trigger.triggerName == "BLUE"){
		//currentColor = ofColor(0,0,255);
	}
}
void testApp::applyTLValues()
	{
	 Particle::speed		=timeline.getKeyframeValue("speed");
	 Particle::spread		=timeline.getKeyframeValue("spread");
	 Particle::viscosity	=timeline.getKeyframeValue("viscosity");
	 Particle::independence	=timeline.getKeyframeValue("independence");
	 Particle::indecision	=timeline.getKeyframeValue("indecision");

	 Particle::weightFlockingForce=timeline.getKeyframeValue("weightFlockingForce");;
	 Particle::weightSphericalForce=timeline.getKeyframeValue("weightSphericalForce");;
	 Particle::weightAttractionPointForce=timeline.getKeyframeValue("weightAttractionPointForce");;
	 Particle::weightFunnelForce=timeline.getKeyframeValue("weightFunnelForce");;
	

	}
void testApp::keyPressed(int key) {
	if(key == 'f')
		ofToggleFullscreen();
	if(key == 'c')
		saveFrame=true;
	if (key=='q')
		{
		//Particle::setWeightSphericalForce(1.);
			Particle::weightSphericalForce=1.;
		}
	if (key=='Q')
		{
	//	Particle::setWeightSphericalForce(0.);
		Particle::weightSphericalForce=0.;
		}
	if (key=='w')
		{
			Particle::weightFlockingForce=1.;
		}
	if (key=='W')
		{
		Particle::weightFlockingForce=0.;
		}
	if (key=='e')
		{
			Particle::weightAttractionPointForce=1.;
		}
	if (key=='E')
		{
		Particle::weightAttractionPointForce=0.;
		}
	if (key=='r')
		{
			Particle::weightFunnelForce=1;
		}
	if (key=='R')
		{
		Particle::weightFunnelForce=0.;
		}
	if (key=='1')
		{
			Particle::selectAttractionMesh(0);
		}
	if (key=='2')
		{
			Particle::selectAttractionMesh(1);
		}
	if (key=='a') enableAllConnecter=true;
	if (key=='A') enableAllConnecter=false;
	if (key=='s') enableTrails=true;
	if (key=='S') enableTrails=false;

	// timeline
	if(key == ' '){
		timeline.togglePlay();
	}
	if(key == 'h'){
		timeline.toggleShow();
	}
		
}
