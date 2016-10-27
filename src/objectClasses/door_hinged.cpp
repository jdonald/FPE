#include "door_hinged.h"

door_hinged::door_hinged() {
	yaw = 0.0f;
	close();
}

void door_hinged::live() {
	if(moving)
	{
		float diff = (goalYaw - yaw);
		yaw += diff*0.05f;
		if(diff < 0.001f && diff > -0.001f)
			{ yaw = goalYaw; moving = false; }
		updateInvM();
	}
}

void door_hinged::open() {
	closed = false;
	goalYaw = 90.0f;
	moving = true;
}

void door_hinged::close() {
	closed = true;
	goalYaw = 0.0f;
	moving = true;
}
