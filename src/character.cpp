#include "character.h"

character::character() {
	height = 35.0f; heightInc = height/5.0f;
	eyeHeight = 27.0f;
	width = COLLISION_PLANE_BUFFER + 0.1f;
	warpTo(vector3(0,0,0)); pov.yaw = -90.0f;
}

vector3 character::getEyePos() {
	return pov.pos + vector3(0.0f, 0.0f, elasticY-pov.pos.z);
}

void character::liveBegin() {
	float diff = pov.pos.z+eyeHeight - elasticY;
	elasticYvel += (vel.z - elasticYvel)*0.2f;
	elasticY += diff*0.1f + elasticYvel;

	//lastPos = pov.pos;
	vel.z -= 0.08f;
	pov.moveBy(vel);
}

void character::liveEnd() { lastPos = pov.pos; }

void character::accel(const vector3 &a) {
	vel += a;
}

void character::warpTo(const vector3 &p) {
	pov.pos = p; lastPos = p; vel.zero();
	elasticY = pov.pos.z+eyeHeight; elasticYvel = 0.0f;
}

void character::moveBy(const vector2 &p) {
	pov.moveBy( p/(1.0f+max(0.0f, pov.pos.z+eyeHeight-elasticY)) );
}

void character::forwardBy(float d) {
	pov.forwardBy(d/(1.0f+max(0.0f, pov.pos.z+eyeHeight-elasticY)) );
}

void character::strafeBy(float d) {
	pov.strafeBy(d/(1.0f+max(0.0f, pov.pos.z+eyeHeight-elasticY)) );
}

void character::panTiltTo(const Pov &p) { pov.panTiltTo(p); }

void character::panTiltBy(float p, float t) { pov.panTiltBy(p, t); }

Pov character::getSwayedPov() {
	static float sway =0.0f;
	sway += 0.01f;
	Pov t;
	t.pos = getEyePos();
	t.yaw = pov.yaw + cosf(sway*0.4f);
	t.pitch = pov.pitch + sinf(sway)*1.5f;
	return t;
}

bool character::clipAsFloor(const collisionPlane &cp, const vector3 &up) {
	return cp.clip(lastPos, pov.pos, 0.1f, vel, up); //no slide effect
	//return cp.clip(lastPos, pos, 0.1f, vel); //clip as a slide
}

bool character::clipAsCeiling(const collisionPlane &cp, const vector3 &up) {
	vector3 body(up*height);
	vector3 head = pov.pos + body;
	if(cp.clip(lastPos + body, head, 0.1f))
	{
		vel.z = max(-0.5f, -fabsf(vel.z)); //bounce
		pov.pos = head - body;
		return true;
	}
	pov.pos = head - body;
	return false;
}

bool character::clipAsWall(const collisionPlane &cp, const vector3 &up) {
	vector3 feelerInc(up*heightInc);
	for(float f = 0.0f; f < height; f += heightInc) //sample points along body
	{
		vector3 was, is, feeler(0,0,0);
		//feeler.z = f;
		//feeler = up*f;
		feeler += feelerInc;
		is = was = pov.pos + feeler;
		if(cp.clip(lastPos + feeler, is, width))
		{
			pov.pos += is-was;
			return true; //break;
		}
	}
	return false;
}
