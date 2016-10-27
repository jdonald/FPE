#pragma once

#include "common.h"
#include <Cg/cgGL.h>

#include "vectors.h"
#include "camera.h"
#include "light.h"

class camera;
class light;

class cgShaderSet {
protected:
	CGprogram vertex, fragment;
	CGparameter uModelViewProj, uModel;
	CGparameter uLightFrom, uLightTo, uLightClr, uLightAngle;
	CGparameter uCameraFrom, uTexture;
public:
	cgShaderSet() {}
	void load(char* vname, char* vfunc, char* fname, char* ffunc);
	void enable();
	void enable(const camera &);
	void updateMatrix(const camera&);
	virtual void updateCameraPos(const vector3&);
	void updateLightPos(const vector3&);
	void updateLightDir(const vector3&);
	void updateLightColor(const vector3&);
	void updateLightAngle(float);

	static CGcontext cgContext;
	static CGprofile vert_profile;
	static CGprofile frag_profile;
	static void errorCallback();
	static void disableAll();
};

extern cgShaderSet *currentShaderSet;

class cgShadowShaderSet: public cgShaderSet {
public:
	void enable();
	void enable(const light &);
	void enable(const camera &c) { enable(); updateMatrix(c); }
	void updateCameraPos(const vector3 &v) {} //overload to do nothing (since doesnt bind cameraFrom parameter)
};
