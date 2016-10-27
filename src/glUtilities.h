#pragma once

#include <GL/glut.h>
#include "vectors.h"
#include "utilities.h"

float convertZ(float near, float far, float z);

void glTranslate1p(const vector3 &);

void glColor1p(const vector3 &);

void glVertex1p(const vector3 &);

void glNormal1p(const vector3 &);

void glTexCoord1p(const vector2 &);

void drawSphere ( float rad, int usec, int vsec );