#pragma once

#include <GL/glut.h>

class timer
{
	static void tick(int a = 0);
	static int period; //in milliseconds
	static bool enabled;

	public:
		void start(int);
		void stop() { enabled = false; }
};

bool timer::enabled = false;
int timer::period = 1000;

void timer::tick(int a) {
	if (!enabled) return;
	glutTimerFunc(period, tick, 0);

	glutPostRedisplay(); //hardcoded func
}

void timer::start(int p) {
	period = p;
	enabled = true;
	glutTimerFunc(period, tick, 0);
}
