#pragma once

#include "../object.h"

class door_hinged: public object {
public:
	door_hinged();
	void init() { object::init(); load(std::ifstream("rsc/door.txt")); }
	void live();
	void open();
	void close();
	void toggle() { if(closed) open(); else close(); }
private:
	bool closed, moving;
	float goalYaw;
};
