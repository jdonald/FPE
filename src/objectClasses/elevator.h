#pragma once

#include "door_sliding.h"

class door_elevator: public door_sliding {
	friend class elevator;
public:
	door_elevator() { counter = 0; }
	void live() { if(isOpen() && ++counter > 350) door_sliding::close(); door_sliding::live(); }
	void open() { counter = 0; door_sliding::open(); }
	void close() { counter = 1000; }
private:
	int counter;
};

class elevator: public object {
public:
	elevator();
	void init();
	void live();
	void draw(const camera &);
	void stencilShadow(const vector3 &);
	bool clip(character &);
	void callToFloor(int);
private:
	static float speed, accel;
	bool moving;
	int floor, goalFloor;
	float goalZ;
	vector3 vel;
	//light light;
	vector3 door1Pos, door2Pos;
	door_elevator door1, door2;
};
