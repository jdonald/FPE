#include "matrix.h"

void matrix::loadModelView() {
	glGetFloatv(GL_MODELVIEW_MATRIX, m);
}

vector3 matrix::operator*(const vector3 &v) const {
	return vector3(
		m[ 0]*v.x + m[ 4]*v.y + m[ 8]*v.z + m[12],
		m[ 1]*v.x + m[ 5]*v.y + m[ 9]*v.z + m[13],
		m[ 2]*v.x + m[ 6]*v.y + m[10]*v.z + m[14]);
}
