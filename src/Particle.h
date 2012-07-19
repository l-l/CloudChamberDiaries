#pragma once

#include "Interactor.h"
#include "ofMain.h"

#include "ofxAssimpModelLoader.h"


inline void randomize(ofVec3f& v) {
	v.x = ofRandomf();
	v.y = ofRandomf();
	v.z=  ofRandomf();
	v.normalize();
}

class Force {
public:
	ofVec3f position;
	float magnitude;
	Force(ofVec3f position, float magnitude) {
		this->position = position;
		this->magnitude = magnitude;
	}
};

class Particle {
public:

	ofVec3f position, velocity, force, localOffset;
	ofVec3f attractionPoint; // point (from the 3d model) 
	vector<ofVec3f> trail;
	
	static int maxTrailSize;
	static int curTrailSize;

	static ofVec3f centeringForce, globalOffset, externalForce;
	static ofVec3f avgPos; //average position of all particles, for the centering force
	static float speed, spread, viscosity, independence, rebirthRadius, neighborhood, confusion, indecision, age, globalDirection;
	static vector<Particle> particles;
	static vector<Force> forces;
	static vector<Interactor> interactors;
	static void setup(int n, float radius);
    static void addForce(ofVec3f position, float magnitude);
    static void updateAll(float dt);

	//drawing different elements
	static void drawParticles();
	static void drawLines();
	static void drawConnecter();

	static void drawTrails();
	static void enableTrails();
	static void disableTrails();
	
	static void drawAllConnecter();
	static void drawInteractors();
	static void setWeightSphericalForce(float w);
	
	

	// setting the weights for the forces
	

	// to control the influence of the various forces (0...1)
	static float
		weightCenteringForce,
		weightExternalForce,
		weightFlockingForce,
		weightSphericalForce,
		weightAttractionPointForce,
		weightFunnelForce,

	// minimum and maximum Length of the connections between particles
		allConnecterMinLength,
		allConnecterMaxLength;


	static ofxAssimpModelLoader model;
    
    static ofMesh partMesh;				// the particles as a mesh
	
	static vector<ofMesh> objMeshes;	// meshes of loaded 3d objects for attraction points
	
	// Trails
	static bool doTrail;
	static bool endingTrails; // indicates that the maxTrailSize should be decremented each update, set by endTrails();
	
	static void applyAttractionPointsFromMesh(ofMesh mesh);
	static void selectAttractionMesh(int i);
	
	Particle(float radius) {
    randomize(localOffset);
  	randomize(position);
  	position *= radius;

  }

// handling the trails
  inline void updateTrail(ofVec3f newPos)
	  {if (ofGetFrameNum()%2==0)   // update the trail position only every 2nd frame
		  {
		  trail.insert(trail.begin(),newPos);
		  if (trail.size()>=curTrailSize)  //was maxTrailSize
			  {trail.pop_back();
			  }
		  }
	  else							// update the first position every frame, in order to have the trail allways connnected to the particle
	    {if (trail.size()>0) trail[0]=newPos;
		else trail.push_back(newPos);
		}
	  }



  inline void applyFlockingForce() 
	{
		float basex = position.x / neighborhood;
		float basey = position.y / neighborhood;
		float basez = position.z / neighborhood;
    force.x +=(
				ofNoise(
						basex + globalOffset.x + localOffset.x * independence,
						basey,
						basez)
				-.5
			  )
				* weightFlockingForce;
	force.y +=
				(ofNoise(
						basex,
						basey + globalOffset.y  + localOffset.y * independence,
						basez
					   )
				  -.5
				)
				 *weightFlockingForce;
	force.z +=(
			  ofNoise(
						basex,
						basey,
						basez + globalOffset.z  + localOffset.z * independence
					  )
				-.5
			   )
					*weightFlockingForce;
	}



  inline void applyViscosityForce() {
    force += velocity * -viscosity;
  }

  inline void applyCenteringForce() 
	  {
		centeringForce = position;
//		centeringForce-=(avgPos).;  //////...processing // avg = (sum of all particle positions) / (amount of particles)

		float distanceToCenter = centeringForce.length();
		
		centeringForce.normalize();
		centeringForce *= -distanceToCenter / (spread * spread);
		force += centeringForce;
	  }


	inline void applyExternalForce() {
		for(int i = 0; i < forces.size(); i++) {
			externalForce = position - forces[i].position;
			float distanceToForce = externalForce.length();
			externalForce.normalize();
			externalForce *= forces[i].magnitude / distanceToForce;
			force += externalForce;		
		}
	}

	// force applied to every particle to keep it on a sphere surface
  	inline void applySphericForce() 
		{
		 float r=200.;		// radius of the sphere
		 float len= position.length()-r;
		 force-=(len/5000.)*position*weightSphericalForce;
		
		}
	// force aiming for the particles's attraction point
  	inline void applyAttractionPointForce() 
		{
		 ofVec3f f=(attractionPoint-position)/30.;//(position-attractionPoint)/1000.;ofVec3f(0,0,0)
		 f=f/(f.length()+1.);
		// f.normalize();
		 force+=f*weightAttractionPointForce;
		
		}  
	inline void applyFunnelForce() 
		{/* //looky
			ofVec3f f= -ofVec3f(position.x,position.y,0); // centering xy force
			float centerD=abs(f.length());
			f=f.getNormalized()*(centerD/350.);
			f.z=1.5/(centerD+0.001) ;
			force+=f;
			*/
			
			
			
			ofVec3f f= -ofVec3f(position.x,position.y,0); // centering xy force
			float centerD=abs(f.length());
		
			float r=30000./(abs(position.z+0.)+100.);//(abs(position.z)+0.01);//1./pow(2,position.z/150.); //target r

			f=f.getNormalized()*(centerD-r)  / ((1.-weightFunnelForce)*10.+1);
			f.z=pow(2,abs(position.z+0.)/100.);
			velocity.scale(1-weightFunnelForce);
			force+=f*weightFunnelForce;
		}

	inline void applyInteractors()
	{float scale=100.;

	 for (int i=0;i<interactors.size();i++)
		{
			force+=interactors[i].getForce(position);

		}
			
		
	}


  inline void update(float dt) {
    force.set(0, 0,0);
    
    applyViscosityForce();
	
	applyCenteringForce();
	applyExternalForce();
	//applyInteractors();

	if (weightFlockingForce>0) applyFlockingForce();
	if (weightSphericalForce>0) applySphericForce();			// new
    if (weightAttractionPointForce>0) applyAttractionPointForce() ;	// new
  	if (weightFunnelForce>0) applyFunnelForce() ;	// new
	
	velocity += force * dt; // mass = 1, acceleration = force
    position += velocity * speed * dt;
	if (position.length()>580)//(position.x>400 || position.x<-400 || position.x>400 || position.y<-400 || position.y>400 || position.z<-400) 
		{
		 randomize(position);
		 position=position*250.;
		 position.z=0;
		 position.y*=1.6;
			/*
		// position reset for the funnel
		 float ang=ofRandom(0.,6.283);
		 position=ofVec3f(sin(ang)*250.,cos(ang)*250.,0.);
		 velocity=ofVec3f(0,0,0);
		 */
		 trail.clear();
		
		}
	if (doTrail) 
		{updateTrail(position)		;
		
		}
	// position.z=0;
  }
};



