#include "door_sliding.h"

door_sliding::door_sliding() {
	locked = false;
	close();
	velFactor = 0.0f;
	slideDirection = 1.0f;
}

void door_sliding::live() {
	if(moving) {
		velFactor += (1.0f-velFactor)*0.02;
		float diff = (goalX - pos.x);
		pos.x += diff*0.05f*velFactor;
		if(diff < 0.05f && diff > -0.05f)
			{ pos.x = goalX; moving = false; velFactor = 0.0f; }
		updateInvM();
	}
}

void door_sliding::open() {
	if(locked && isClosed()) return;
	closed = false;
	goalX = 14.5f*slideDirection;
	moving = true;
}

void door_sliding::close() {
	closed = true;
	goalX = 0.0f;
	moving = true;
}
