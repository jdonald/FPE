#include "camera.h"

const float camera::eyeSeperation = 5.0f;

Pov::Pov():pos(0,0,0) { pitch = yaw = 0.0f; }

Pov::Pov(const Pov &p):pos(p.pos) { pitch = p.pitch; yaw = p.yaw; }

Pov::Pov(const vector3 &v):pos(v) { pitch = yaw = 0.0f; }

void Pov::panTiltBy(float p, float t) { yaw += p; pitch += t; }
void Pov::panTiltTo(float p, float t) { yaw = p; pitch = t; }
void Pov::panTiltTo(const Pov &p) { yaw = p.yaw; pitch = p.pitch; }

void Pov::moveBy(const vector3 &m) { pos += m; }

void Pov::moveTo(const vector3 &p) { pos = p; }

void Pov::forwardBy(float d) {
	float a = yaw*deg2rad;
	pos += vector2(sinf(a)*d, cosf(a)*d);
}

void Pov::strafeBy(float d) {
	float a = yaw*deg2rad;
	pos += vector2(cosf(a)*d, -sinf(a)*d);
}


camera::camera() {
	aspect = 4.0/3.0; viewDepth = 1500.0; zNear = 1.0;
	DoF_xRot = DoF_yRot = 0.0f;
	DoF_x = DoF_y = 0.0f; //temp
}

void camera::init() {
	setupProjectionMatrix();
	updateMatrix();
}

void camera::setupProjectionMatrix() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(eyeOffset) gluPerspective(60, aspect/2.0f, zNear, viewDepth+zNear);
	else gluPerspective(60, aspect, zNear, viewDepth+zNear);
	glMatrixMode(GL_MODELVIEW);
}

void camera::setAspect(double a) {
	aspect = a; init();
}

void camera::panTiltBy(float p, float t) {
	pov.panTiltBy(p, t); updateMatrix();
}

void camera::moveBy(const vector3 &m) {
	pov.moveBy(m); updateMatrix();
}

void camera::moveTo(const vector3 &p) {
	pov.moveTo(p); updateMatrix();
}

void camera::forwardBy(float d) {
	pov.forwardBy(d); updateMatrix();
}

void camera::strafeBy(float d) {
	pov.strafeBy(d); updateMatrix();
}

void camera::updateMatrix() {
	//glPushMatrix();
	//	glLoadIdentity();
	//	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	//	glRotatef(pitch, 1.0f, 0.0f, 0.0f);
	//	glRotatef(yaw, 0.0f, 0.0f, 1.0f);
	//	glTranslatef(-pos.x, -pos.y, -pos.z);
	//	invM.loadModelView();
	//glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(eyeOffset, 0, 0); //for stereoscopic rendering
		glTranslatef(DoF_x, DoF_y, 0);
		glTranslatef(0,0,-DoF_focalDistance.getFDValue());
		glRotatef(DoF_xRot, 0.0f, 1.0f, 0.0f);
		glRotatef(DoF_yRot, 1.0f, 0.0f, 0.0f);
		glTranslatef(0,0,DoF_focalDistance.getFDValue());
		glRotatef(pov.pitch-90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(pov.yaw, 0.0f, 0.0f, 1.0f);
		glTranslatef(-pov.pos.x, -pov.pos.y, -pov.pos.z);
		view.loadModelView();
	glPopMatrix();

	//float y = yaw*deg2rad, p = pitch*deg2rad, sp = sinf(p);
	//up.x = -sinf(y)*sp;
	//up.y = cosf(y)*sp;
	//up.z = cosf(p);

	//if(currentShaderSet) currentShaderSet->updateCameraPos(pov.pos);
}

vector3 camera::unTransform(const vector3 &v) const {
	return invM * v;
}

void camera::preMultView() const {
	matrix m; m.loadModelView();
	glLoadMatrixf ( view.m );
	glMultMatrixf ( m.m );
}

void camera::DoF_1(void* w, void(*sceneRenderFunc)(void*) ) {
	glClear(GL_ACCUM_BUFFER_BIT); //glAccum(GL_MULT, .9f);
	DoF_focalDistance.update();
	camera old = *this;
	
	const int passes = 16;
	int r = 1; float a = 0.f, tilt = M_2_PI * 20.f/360.f;
	for(int i = 0; i < passes; ++i) {
		a += M_PI_2; if(a > M_2_PI) { ++r; a -= M_2_PI; tilt *= -1.f; }
		DoF_xRot = cosf(tilt + a)*r*.05f;
		DoF_yRot = sinf(tilt + a)*r*.05f;
		updateMatrix();
		sceneRenderFunc(w);
		glAccum(GL_ACCUM, 1.f/passes);
	}
	*this = old;
	glAccum(GL_RETURN, 1.f);
}

void camera::DoF_2(void* w, void(*sceneRenderFunc)(void*) ) {
	const int passes = 16;

	double sliceWidth = viewDepth / passes;
	double n = viewDepth + zNear - sliceWidth,
		   f = viewDepth + zNear;

	camera old = *this;
	for(int i = 0; i < passes; ++i) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, aspect, n, f);
		glMatrixMode(GL_MODELVIEW);

		//DoF_x, DoF_y
		sceneRenderFunc(w);
		//blur this slice by (distance from center slice)

		n -= sliceWidth; f -= sliceWidth;
	}
	*this = old;
}
