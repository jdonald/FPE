#pragma once

extern bool drawOutlines;

#include <vector>
#include "common.h"

#include "utilities.h"
#include "glUtilities.h"
#include "vectors.h"

struct face;

struct shadowWall {
	face *a, *b;
	unsigned char edgeA, edgeB;
};

struct vertex {
	vector3 xyz;

	vertex(const vector3 &xyz):xyz(xyz) {}
};

struct face {
	face(unsigned int, unsigned int, unsigned int, const std::vector<vertex> &);
	~face();

	unsigned int i[3];
	vector3 normal;
	//planeEquation planeEq;
	shadowWall *shadow[3];
private:
	face() {}
};

class model {
	vector3 clr;
	std::vector<vertex> vertexList;
	std::vector<face*> faceList;
	GLuint glListID;

	void generateShadowData();
public:
	model():clr(1,1,1) { glListID = 0; }
	~model() { clear(); glDeleteLists(1, glListID); }

	void addVertex(const vector3 &);
	void addFace(unsigned int a, unsigned int b, unsigned int c);

	void load(std::istream &);
	void clear();
	void cache();
	void draw();
};