#include "elevator.h"

float elevator::speed = 1.3f;
float elevator::accel = 0.005f;

elevator::elevator():vel(0.0f,0.0f,0.0f), door1Pos(0.0f, -25.5f, 0.0f), door2Pos(-15.0f, -25.5f, 0.0f) {
	floor = 1; goalFloor = floor;
	moving = false;
	//light.setAmbient(0.0f, 0.0f, 0.0f);
	//light.setDiffuse(0.710f, 0.827f, 1.0f);
	//light.setSpecular(0.6f, 0.6f, 0.7f);
}

void elevator::init() {
	object::init();
	load(std::ifstream("rsc/Elevator.txt"));
	//light.setPos(pos+vector3(0.0f,0.0f,45.0f), true);
	door1.init();
	door2.init(); door2.slideLeft();
}

void elevator::callToFloor(int f) {
	goalFloor = f;
	goalZ = 0.0f + (f-1)*52.0f;
	if(floor == goalFloor) { door1.open(); door2.open(); return; }
	moving = true;
	door1.close(); door2.close();
}

void elevator::live() {
	//if(keys['V']) { door1.open(); door2.open(); }
	//for(unsigned int i = 1; i <= 9; ++i)
	//	if(keys[i+'0']) callToFloor(i);

	door1.live(); door2.live();

	if(moving && door1.isClosed() && door2.isClosed())
	{
		//light.setPos(pos+vector3(0.0f,0.0f,45.0f), true);
		door1.lock(); door2.lock();
		float diff = goalZ - pos.z;
		float dir = sign(goalZ - pos.z);
		if(dir < 0)
		{
			if(vel.z > -speed) vel.z -= accel;
			vel.z = max(vel.z, diff*0.02f);
		}
		else
		{
			if(vel.z < speed) vel.z += accel;
			vel.z = min(vel.z, diff*0.02f);
		}
		pos += vel;
		if(diff < 0.1f && diff > -0.1f)
		{
			pos.z = goalZ; vel.z = 0.0f; moving = false;
			door1.unlock(); door2.unlock();
			door1.open(); door2.open();
			floor = goalFloor;
		}
		for(unsigned int i = 0; i < collisionPlanes.size(); ++i)
			collisionPlanes[i].setVel(vel);
		updateInvM();
	}
}

void elevator::draw(const camera &cam) {
	//light.enable();
	object::draw(cam);
	
	door1.pos += pos+door1Pos;
	door1.draw(cam);
	door1.pos -= pos+door1Pos;
	
	door2.pos += pos+door2Pos;
	door2.draw(cam);
	door2.pos -= pos+door2Pos;
	//light.disable();
}

void elevator::stencilShadow(const vector3 &lp) {
	//object::stencilShadow(lp);
	door1.pos += pos+door1Pos;
	door2.pos += pos+door2Pos;
	door1.updateInvM(); door2.updateInvM();
	//door1.stencilShadow(lp);
	//door2.stencilShadow(lp);
	door1.pos -= pos+door1Pos;
	door2.pos -= pos+door2Pos;
	door1.updateInvM(); door2.updateInvM();
}

bool elevator::clip(character &c) {
	bool clip = object::clip(c);

	door1.pos += pos+door1Pos;
	door2.pos += pos+door2Pos;
	if(door1.clip(c) || door2.clip(c))
		{ door1.open(); door2.open(); clip = true; }
	door1.pos -= pos+door1Pos;
	door2.pos -= pos+door2Pos;
	return clip;
}
