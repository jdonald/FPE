#pragma once

#include "common.h"
#include "vectors.h"
#include "matrix.h"
#include "shaders.h"
#include "tweenPID.h"

struct Pov {
	vector3 pos;
	float pitch, yaw;

	Pov();
	Pov(const Pov &);
	Pov(const vector3 &);
	void panTiltBy(float, float);
	void panTiltTo(float, float);
	void panTiltTo(const Pov &);
	void moveBy(const vector3 &);
	void moveTo(const vector3 &);
	void forwardBy(float);
	void strafeBy(float);
};

class camera {
	GLdouble viewDepth, zNear;

	Pov pov;
	double aspect;
	matrix invM, view;
	float eyeOffset;
	static const float eyeSeperation;

	tweenPID DoF_focalDistance;
	float DoF_xRot, DoF_yRot;	//method 1
	float DoF_x, DoF_y;			//method 2 (temp)

	void setupProjectionMatrix();
public:
	camera();
	void init();

	Pov getPov() { return pov; }
	void setPov(Pov p) { pov = p; updateMatrix(); }
	void setAspect(double a);
	void panTiltBy(float p, float t);
	void moveBy(const vector3 &);
	void moveTo(const vector3 &);
	void forwardBy(float d);
	void strafeBy(float d);

	void leftEye() { eyeOffset = eyeSeperation/2.0f; init(); } //eyes cross
	void rightEye() { eyeOffset = -eyeSeperation/2.0f; init(); }
	void center() { eyeOffset = 0.0f; init(); }

	void updateMatrix();
	vector3 unTransform(const vector3 &) const;
	void preMultView() const;

	vector3 getPos() const { return pov.pos; }

	void DoF_1(void*, void(*sceneRenderFunc)(void*) );
	void DoF_2(void*, void(*sceneRenderFunc)(void*) );
	void setFocalDistance(float d) { DoF_focalDistance.setFDSetPoint(d); }
};
