#include "tweenPID.h"

tweenPID::tweenPID() {
	Kp = 0.2f; 
	Ki = 0.02f; 
	Kd = 0.001f;
	init(0);
}

tweenPID::tweenPID(float K_p, float K_i, float K_d) {
	Kp = K_p; 
	Ki = K_i; 
	Kd = K_d;
	init(0);
}

tweenPID::tweenPID(float Kp1, float Ki1, float Kd1, float setPoint){
	init(setPoint);
}

float tweenPID::getFDSetPoint() { return fd_SetPoint; }

void tweenPID::setFDSetPoint(float setPoint) {
	fd_SetPoint = setPoint;
}

float tweenPID::getFDValue() { return fd_Value; }

void tweenPID::setKp(float K_p) { Kd = K_p; }
void tweenPID::setKi(float K_i) { Ki = K_i; }
void tweenPID::setKd(float K_d) { Kd = K_d; }

void tweenPID::update() {
	e_past = e;
	e = fd_SetPoint-fd_Value;
	float de = e-e_past;
	fd_Integral += e;
	fd_Value += Kp*e + Ki*fd_Integral + Kd*de;
}


void tweenPID::init(float setInit) {
	fd_Integral = 0;
	fd_SetPoint = setInit;
	fd_Value = setInit;
	e = 0;
	e_past = 0;
}


