#include "Interactor.h"




Interactor::Interactor(ofVec3f _pos)
{maxRadius=100;
 minRadius=50;
 strength=3.5;
 pos=_pos;
 cnt=0;
}
Interactor::Interactor(ofVec3f _pos, float _minRadius, float _maxRadius, float _strength)
{maxRadius=_maxRadius;
 minRadius=_minRadius;
 strength=_strength;
 pos=_pos;
 cnt=0;
}

Interactor::~Interactor(void)
{
}

void Interactor::update(void)
	{cnt+=0.01;
	}

ofVec3f Interactor::getForce(ofVec3f remotePoint)
	{	
		pos+=ofVec3f(0,(sin(strength*cnt))*0.001,0);
		ofVec3f f= (pos-remotePoint);
		float len=abs(f.length());
		if (len<maxRadius && len>minRadius)
			{
			f.normalize();
			f.scale(strength/max(1.,(len/50.)));


			}
		else
			f= ofVec3f(0,0,0);
		return f;
	}