#pragma once

#include "ofMain.h"


//spherical interactor
class Interactor
{
public:
	Interactor(ofVec3f pos);
	Interactor(ofVec3f _pos, float _minRadius, float _maxRadius, float _strength);
	~Interactor(void);

	ofVec3f pos;
	float
		maxRadius,
		minRadius,
		strength;

	float cnt;

	ofVec3f getForce(ofVec3f remotePoint);
		
	 void update(void);
};

