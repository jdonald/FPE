#pragma once

#include "common.h"
#include "vectors.h"

struct matrix {
	float m[16];

	void loadModelView();
	vector3 operator*(const vector3&) const;
};
