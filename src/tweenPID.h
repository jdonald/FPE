#pragma once

class tweenPID{
public:
	tweenPID();
	tweenPID(float K_p, float K_i, float K_d);
	tweenPID(float Kp1, float Ki1, float Kd1, float setPoint);

	float getFDSetPoint();
	void  setFDSetPoint(float setPoint);
	float getFDValue();

	void setKp(float K_p);
	void setKi(float K_i);
	void setKd(float K_d);

	void update();
protected:
	float Kp, Ki, Kd;
	float fd_SetPoint;
	float fd_Value;
private:
	void init(float setInit);
	float e;
	float e_past;
	float fd_Integral;
};
