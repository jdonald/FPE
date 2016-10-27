#pragma once

#include "common.h"
#include "vectors.h"
#include "model.h"
#include "collisionPlane.h"
#include "camera.h"
#include "character.h"

class object
{
	model mesh;
	matrix invM, mat;
	vector3 up;
protected:
	vector3 pos;
	float yaw, pitch;
	std::vector<collisionPlane> collisionPlanes;

	void updateInvM();
public:
	object();
	virtual void init() {}
	virtual void load(std::istream &, std::string = "");
	virtual void live() {}
	virtual void draw(const camera &);

	virtual void setPos(const vector3 &p) { pos = p; }
	virtual bool clip(character &);
	virtual void reCache();
};
