#pragma once

#include "utilities.h"
#include "collisionPlane.h"
#include "camera.h"

class character {
	vector3 vel;
	float height, eyeHeight, heightInc, width;
	float elasticY, elasticYvel;
public:
	character();
	vector3 getEyePos();
	void liveBegin();
	void liveEnd();
	
	void accel(const vector3 &);
	void warpTo(const vector3 &);
	void moveBy(const vector2 &);
	void forwardBy(float d);
	void strafeBy(float d);
	void panTiltTo(const Pov &);
	void panTiltBy(float, float);
	Pov getSwayedPov();
	
	bool clipAsFloor(const collisionPlane &, const vector3 &up = vector3(0.0f,0.0f,1.0f));
	bool clipAsCeiling(const collisionPlane &, const vector3 &up = vector3(0.0f,0.0f,1.0f));
	bool clipAsWall(const collisionPlane &, const vector3 &up = vector3(0.0f,0.0f,1.0f));

	vector3 lastPos;
	Pov pov;
};
