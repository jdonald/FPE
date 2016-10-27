#pragma once

#include "../object.h"

class door_sliding: public object {
public:
	door_sliding();
	void init() { object::init(); load(std::ifstream("rsc/ElevatorDoor.txt")); }
	void live();
	void open();
	void close();
	void toggle() { if(closed) open(); else close(); }
	void lock() { locked = true; }
	void unlock() { locked = false; }
	bool isClosed() { return closed && !moving; }
	bool isOpen() { return !closed && !moving; }
	void slideLeft() { slideDirection = -1.0f; }
	void slideRight() { slideDirection = 1.0f; }
private:
	bool closed, moving, locked;
	float goalX, velFactor, slideDirection;
};
