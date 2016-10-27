#include "light.h"

light::light():clr(1,1,1) { enabled = true; }

void light::load(std::istream &in, std::string path) {}

void light::use() const {
	currentShaderSet->updateLightColor(clr);
	currentShaderSet->updateLightPos(pos);
}

spotlight::spotlight() {
	pitch = yaw = 0.0f; isPitchYaw = false;
}

void spotlight::load(std::istream &in, std::string path) {
	char ch;
	
	skipWhitespace(in);
	while(!in.eof() && in.get(ch))
	{
		switch(ch)
		{
		case '/': //comment
			do { in.get(ch); } while(ch != '\n' && !in.eof());
		break;
		case 'c': //color
			in >> clr;
		break;
		case 'p': //position
			in >> pos;
		break;
		case 'd': //direction
			in >> dir;
		break;
		case 'a': //angle
			in >> angle;
			angle *= deg2rad;
		break;
		case '}': //end of spotlight
			return;
		break;
		}
		skipWhitespace(in);
	}
}

void spotlight::setPanTilt(float p, float t) {
	yaw = p; pitch = t;
	p *= deg2rad; t *= deg2rad;
	dir = vector3(0,-1,0);
	dir.rotX(t);
	dir.rotZ(p);
	//float cx = cosf(t), sx = sinf(t), cz = cosf(p), sz = sinf(p);
	//dir = vector3( -sz, cx*cz, sx*cz );
}

void spotlight::use() const {
	light::use();
	if(isPitchYaw) currentShaderSet->updateLightDir(dir);
	else currentShaderSet->updateLightDir((pos-dir).normal());
	currentShaderSet->updateLightAngle(angle);
}
