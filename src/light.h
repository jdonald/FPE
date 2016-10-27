#pragma once

#include "utilities.h"
#include "vectors.h"
#include "shaders.h"
#include "camera.h"

class light {
public:
	vector3 pos, clr;
	bool enabled;
public:
	light();
	virtual void load(std::istream &, std::string);

	virtual void use() const;
};

class spotlight: public light {
public:
	vector3 dir;
	float angle, pitch, yaw;
	bool isPitchYaw;
public:
	spotlight();
	void load(std::istream &, std::string);
	void setPanTilt(float, float);
	void use() const;
};
