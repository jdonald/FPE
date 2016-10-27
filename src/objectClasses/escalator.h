#pragma once

#include "../object.h"

class escalator: public object {
public:
	void init();
	void setPos(const vector3 &);
	void live();
	void draw(const camera &);
	//void stencilShadow(const point3d &);
private:
	static vector3 dir;
	object stairs, stair1, stair2;
	vector3 rampPos;
};
