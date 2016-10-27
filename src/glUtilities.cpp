#include "glUtilities.h"

float convertZ(float near, float far, float z) {
    // Convert Z from [0, 1] to [-1, 1]
    float wz = (2.0f * z) - 1.0f;
    // Inverse projection matrix on the Z coordinate (assuming W=1)
    float a = -(far - near) / (2.0f * far * near);
    float b =  (far + near) / (2.0f * far * near);
    return 1.0f / (wz * a + b);
}

void glTranslate1p(const vector3 &p) { glTranslatef(p.x, p.y, p.z); }

void glColor1p(const vector3 &c) { glColor3f(c.x, c.y, c.z); }

void glVertex1p(const vector3 &p) { glVertex3fv(&p.x); }

void glNormal1p(const vector3 &p) { glNormal3fv(&p.x); }

void glTexCoord1p(const vector2 &p) { glTexCoord2d(p.x, p.y); }

void drawSphere ( float rad, int usec, int vsec ) {
	float vstep = 180.0 / vsec;
	float ustep = 360.0 / usec;
	float x, y, z, w;
	float xl, yl, zl, wl; 
	glBegin ( GL_TRIANGLE_STRIP );	
	xl = sin(-90 * deg2rad); wl = cos(-90 * deg2rad);
	for (float vang = -90.0; vang <= 90.0; vang += vstep) {
		x = sin(vang * deg2rad); w = cos(vang * deg2rad);
		for (float uang =0; uang <= 360.0; uang += ustep) {			
			yl = wl * cos(uang * deg2rad); zl = wl * sin(uang * deg2rad);
			y = w * cos(uang * deg2rad); z = w * sin(uang * deg2rad);			
			glTexCoord2f ( 0.5 + (vang-vstep)/180.0, uang/360.0f  ); glNormal3f ( xl, yl, zl ); glVertex3f ( xl*rad, yl*rad, zl*rad );
			glTexCoord2f ( 0.5 + vang/180.0, uang/360.0f ); glNormal3f ( x, y, z ); glVertex3f ( x*rad, y*rad, z*rad );
		}
		xl = x; wl = w;
	}
	glEnd ();
}