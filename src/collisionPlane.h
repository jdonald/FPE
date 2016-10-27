#pragma once

#include <vector>
//#include "common.h"

#include "utilities.h"
#include "glUtilities.h"

struct planeEquation {
	void calcPlane(const vector3 &v1, const vector3 &v2, const vector3 &v3) {
		a = v1.y*(v2.z-v3.z) + v2.y*(v3.z-v1.z) + v3.y*(v1.z-v2.z);
		b = v1.z*(v2.x-v3.x) + v2.z*(v3.x-v1.x) + v3.z*(v1.x-v2.x);
		c = v1.x*(v2.y-v3.y) + v2.x*(v3.y-v1.y) + v3.x*(v1.y-v2.y);
		d = -( v1.x*(v2.y*v3.z - v3.y*v2.z) + v2.x*(v3.y*v1.z - v1.y*v3.z) + v3.x*(v1.y*v2.z - v2.y*v1.z) );
		normal = vector3(a,b,c).normal();
		dNormal = d / sqrtf(a*a+b*b+c*c);
	}
	float distance(const vector3 &p) const { return normal.x*p.x + normal.y*p.y + normal.z*p.z + dNormal; }
	bool inFront(const vector3 &p) const { return a*p.x + b*p.y + c*p.z + d > 0; }
	vector3 normal;
	float a, b, c, d, dNormal;
};

class collisionPlane
{
	bool dynamic, floor, ceiling;
	void refactor();
	vector3 pos, vel;
	vector3 up, right; //unit vectors for orientation
	planeEquation plane;
	std::vector< std::vector<vector3> > vertexLists;
	std::vector< std::vector<vector2> > originals;

	bool insidePoly(unsigned int, const vector3 &) const;

public:
	collisionPlane();
	void load(std::istream &);
	void setPos(const vector3 &p) { pos = p; }
	void setVel(const vector3 &v) { vel = v; }
	vector3& getVel() { return vel; }
	void makeDynamic() { dynamic = true; }
	void setUpAndRight(const vector3 &, const vector3 &);
	void addVertex(const vector2 &);
	void addVertex(const vector3 &);
	void addMargin(float, float); //scalars for each axis of plane surface
	void addHole();
	void live();
	void draw() const;
	//bool inFront(const point3d &p) { return plane.inFront(p-pos); }
	//bool inFront(const point3d &p, float threshold) { return plane.distance(p-pos) > threshold; }
	bool clip(const vector3 &, vector3 &, float) const;
	bool clip(const vector3 &, vector3 &, float, vector3 &) const;
	bool clip(const vector3 &, vector3 &, float, vector3 &, const vector3 &) const;

	bool isFloor() const { return floor; }
	void isFloor(bool b) { floor = b; }
	bool isCeiling() const { return ceiling; }
	void isCeiling(bool b) { ceiling = b; }
};
