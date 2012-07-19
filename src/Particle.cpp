#include "Particle.h"

#include "ofxAssimpModelLoader.h"



vector<Force>
	Particle::forces;
	
vector<Particle>
	Particle::particles;
vector<ofMesh>
	Particle::objMeshes;

ofVec3f //ofVec3f
	Particle::centeringForce,
	Particle::globalOffset,
	Particle::externalForce;

ofVec3f
	Particle::avgPos;

float
	Particle::speed,
	Particle::spread,
	Particle::viscosity,
	Particle::independence,
	Particle::neighborhood,
	Particle::confusion,
	Particle::indecision,
	Particle::age,
	Particle::globalDirection,
	
	Particle::weightCenteringForce,

	Particle::weightExternalForce,
	Particle::weightFlockingForce,
	Particle::weightSphericalForce,
	Particle::weightAttractionPointForce,
	Particle::weightFunnelForce,

	Particle::allConnecterMinLength,
	Particle::allConnecterMaxLength;
	
	vector<Interactor>	Particle::interactors;

	ofxAssimpModelLoader Particle::model;
	ofMesh Particle::partMesh;


int Particle::maxTrailSize;
int Particle::curTrailSize;
	
bool Particle::doTrail;
bool Particle::endingTrails;


void Particle::setup(int n, float radius) 
	{	// allConnecter
		allConnecterMinLength=2;
		allConnecterMinLength=30;

		// trails
		maxTrailSize=	20;
		curTrailSize=	0;
		doTrail=true;

		// particles, the initial particle behaviour settings
		globalOffset.set(1. / 3, 2. / 3,1.);
		speed =			1.1; //1;
		spread =		25;//25;
		viscosity =		.1; //.1;
		independence =	0.5; //1
		neighborhood =	600.; //100
		confusion =		.01; //.01;
		indecision =	.01;//.01;
		age = 0;
		globalDirection = 0;
		
	
		// force weights, to control the influences of the various forces (! use values between 0...1 !)
		weightCenteringForce=1.;
		weightExternalForce=1.;
		weightFlockingForce=1.;
		weightSphericalForce=1.;
		weightAttractionPointForce=0.5,
		weightFunnelForce=0.;
		
		// 3d models
		model.loadModel("dog.3ds",true);
		objMeshes.push_back(model.getMesh(0));

		model.loadModel("prism.dae",true);
		objMeshes.push_back(model.getMesh(0));


		// Interactors, set up some test interactors with random forces
		for (int k=0;k<5;k++)
			{interactors.push_back(	Interactor(	ofVec3f(ofRandom(-150,150),ofRandom(-250,250),ofRandom(-150,150))	, ofRandom(0,10), ofRandom(30,150),	 ofRandom(-5.,-3.6)	)						);  //interactors.push_back(	Interactor(	ofVec3f(ofRandom(-200,200),ofRandom(-200,200),ofRandom(-200,200))	, ofRandom(0,30), ofRandom(40,200),	 ofRandom(.01,.6)	)						);
			}

		for(int i = 0; i < n; i++)
			{
			 Particle::particles.push_back(Particle(radius));
			}

		//Particle::applyAttractionPointsFromMesh(objMesh);
		Particle::selectAttractionMesh(1);		//select the mesh where the particles move to in attractionPoint mode
	}


void Particle::updateAll(float dt = 1. / 60.) 
	{
		 for (int j=0;j<interactors.size();j++)
			{interactors[j].update();
			}
		//	neighborhood = ofGetAppPtr()->mouseX*1.1;
		//	independence = ofGetAppPtr()->mouseY*0.01; //1

		avgPos= ofVec3f(0,0,0);
	
		for (int i = 0; i < particles.size(); i++) 
			{
				particles[i].update(dt);
				avgPos+=particles[i].position;
			}
		
		avgPos=avgPos/(particles.size()*1.);

		globalDirection += ofSignedNoise(indecision * age);
		ofVec3f direction(0, 1);
		direction.rotate(globalDirection,ofVec3f(1, 0, 0) );
		globalOffset += confusion * direction * dt;
		age += dt;
		forces.clear();

		//TRAILS
		if (endingTrails && curTrailSize>0)  // disableTrails has been called, now we are decrementing the size
			{curTrailSize--;
			}

		else if (endingTrails && curTrailSize==0)	// now it's really over for the trails
			{doTrail=false;
			 endingTrails=false;
			 for (int j = 0; j < particles.size(); j+=1) 
				{particles[j].trail.clear();
				}
			}

		else if (doTrail && !endingTrails && curTrailSize<maxTrailSize) // incrementing the size 
			{curTrailSize++;
			}

	}



// INTERACTOR
// draws the force interactor positions as dots

void Particle::drawInteractors()
	{ofMesh iMesh;
	 iMesh.clear();
	 iMesh.setMode(OF_PRIMITIVE_POINTS); //OF_PRIMITIVE_LINES //OF_PRIMITIVE_POINTS OF_PRIMITIVE_LINE_STRIP
	
	 for(int i = 0; i < interactors.size(); i++) 
		{iMesh.addVertex(interactors[i].pos);
		}
	
	 //ofSetColor(255,255,255,10); //-- managed by shader
	 iMesh.drawVertices();
	
	}

	
// PARTICLES
// draws the particles

void Particle::drawParticles() 
	{
	partMesh.clear();
	//partMesh.setUsage(GL_DYNAMIC_DRAW );
	partMesh.setMode(OF_PRIMITIVE_POINTS); //OF_PRIMITIVE_LINES //OF_PRIMITIVE_POINTS OF_PRIMITIVE_LINE_STRIP
	
	for(int i = 0; i < particles.size(); i++) 
		{
		partMesh.addVertex(particles[i].position);
	    //ofVec3f p=objMesh.getVertex(i%objMesh.getNumVertices());
		//partMesh.addVertex(p*100.);
		}
	
	//ofSetColor(255,255,255,10); //-- managed by shader
	partMesh.drawVertices();
	
	}


// LINES (velocity)
// draw veclocity vectors as lines for every particle
// not really used

 void Particle::drawLines()
	{ofVboMesh lines;
	
	lines.setMode(OF_PRIMITIVE_LINES);

	for(int i = 0; i < particles.size(); i++) 
		{
		lines.addVertex(particles[i].position*1.0);
	 //   lines.addVertex(particles[i].position*particles[i].speed*1.1);
		lines.addColor(ofFloatColor(1.,1.,1.,0.5/particles[i].velocity.squareLength()));  // this only works with ofVboMesh, not with ofMessh
		lines.addVertex(particles[i].position-particles[i].velocity*26.);
		lines.addColor(ofFloatColor(1.,1.,1.,0.));  // this only works with ofVboMesh, not with ofMessh
	//	lines.addColor(ofColor(ofRandom(0,255),ofRandom(0,255),ofRandom(0,255)));

		
		}

	//ofDisableSmoothing();
	//ofSetColor(240,220,255,20);
	ofSetLineWidth(1);   
	lines.drawVertices();
	}


// CONNECTERS
// draw some arbitrary connections between particles
// not really used

void Particle::drawConnecter()
	{ofMesh connecter;
	 connecter.setMode(OF_PRIMITIVE_LINES);
	
	 for(int j = 0; j < 60; j+=2) 
		{
		connecter.addVertex(particles[j].position);
		connecter.addVertex(particles[j+1].position);
		}

	ofEnableSmoothing();
	ofSetLineWidth(2);   
	ofSetColor(220,255,230,200);
	connecter.drawVertices();
	}

void Particle::enableTrails()
	{if (!doTrail)
		{doTrail=true;
		}
	}

void Particle::disableTrails()
	{
	 // doTrail=false;
	 endingTrails=true;  // this will start decrementing the trail lenght (size) til zero, within the updateAll()
	}


// TRAILS
// draws the particle trails, with set trail.size
//

void Particle::drawTrails()
	{ofVboMesh trailMesh;
	 trailMesh.setMode(OF_PRIMITIVE_LINES);
	
	 for(int j = 0; j < particles.size(); j+=1) 
		{if (particles[j].trail.size()>0 && curTrailSize>0)
			{
				for (int k=0;k< MIN( (curTrailSize-1), (particles[j].trail.size()-1) )  ; k++)  //particles[j].trail.size()  //(particles[j].trail.size()-1)
					{trailMesh.addVertex(particles[j].trail[k]);
		 		   	trailMesh.addColor(ofFloatColor(1.,1.,0.9,(1-k/(float)curTrailSize)/6.)); 
					 trailMesh.addVertex(particles[j].trail[k+1]);
					 trailMesh.addColor(ofFloatColor(1.,1.,0.9,(1-(k+1.)/(float)curTrailSize)/6.)); 
					}
			}	
		}

	ofEnableSmoothing();
	ofSetLineWidth(1);   
	//ofSetColor(220,255,230,200);
	trailMesh.drawVertices();
	doTrail=true;

	}


// draws the connection between the particles depending on the distances
 void Particle::drawAllConnecter()
	{
	ofMesh allConnecter;
	allConnecter.setMode(OF_PRIMITIVE_LINES);//OF_PRIMITIVE_LINES); //OF_PRIMITIVE_TRIANGLES //OF_PRIMITIVE_POINTS
	int os=3; // only connect every x particle, to save performance
	float mult=580;
	float osx=500.;
	float osy=0.5;
	 for(int j = 0; j < (particles.size()-os); j+=os) 
		{
			for (int k=j+os;k<(particles.size()-os);k+=os)
			{float len=(particles[j].position-particles[k].position).length();
				if (len>allConnecterMinLength && len<allConnecterMaxLength )
					{allConnecter.addVertex(particles[j].position);
				//	 allConnecter.addNormal(particles[j].position);
				//	 allConnecter.addTexCoord(ofVec3f(	(  (particles[j].position.x/mult+mult/2.)+1920/2),		particles[j].position.y+mult/2+1080/2,0));
			
					 allConnecter.addVertex(particles[k].position);
				//	  allConnecter.addNormal(particles[k].position);
				//	 allConnecter.addTexCoord(ofVec3f(	(  (particles[k].position.x+mult/2.)+1920/2),		particles[k].position.y+mult/2+1080/2,0));
			
					}
				}
		}
	 
	//ofEnableSmoothing();
	 ofDisableSmoothing();
	 glDisable(GL_DEPTH_TEST);
	 ofDisableLighting();
	 ofSetLineWidth(2);   
	//ofSetColor(220,255,230,80);
	 ofSetColor(255,255,255,20);
	 allConnecter.drawVertices();
	
	}



void Particle::addForce(ofVec3f position, float magnitude) 
	{
	forces.push_back(Force(position, magnitude));
	}

void Particle::setWeightSphericalForce(float w) 
	{

	for(int i = 0; i < particles.size(); i++) 
		{
		particles[i].weightSphericalForce=w;
		}

	}

// ATTRACTION POINTS FROM MESH

void Particle::applyAttractionPointsFromMesh(ofMesh mesh)
	{float scale=100.;
	 for (int i = 0; i < particles.size(); i++) 
		{ofVec3f p=mesh.getVertex(i%mesh.getNumVertices());
		 particles[i].attractionPoint=p*scale;
		}
	}

void Particle::selectAttractionMesh(int i)
	{
		Particle::applyAttractionPointsFromMesh(objMeshes[i% (objMeshes.size())]  );
	}
