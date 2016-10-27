#pragma once

#include <fstream>
#include <string>

#define NULL 0

#define _USE_MATH_DEFINES
#include <math.h>
const float deg2rad = (float)M_PI/180.0f;
const float rad2deg = 1.0f/deg2rad;

#include <GL/glut.h>

#define COLLISION_PLANE_BUFFER 6.0f //should match player width

