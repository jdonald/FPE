#include "escalator.h"

vector3 escalator::dir = vector3(0.0f, 84.0f, 52.0f).normal();

void escalator::init()
{
	object::init();
	load(std::ifstream("rsc/Escalator.txt"));
	collisionPlanes[0].setVel(vector3(0.0f, 0.84f, 0.0f).normal()*0.2f);
	collisionPlanes[1].setVel(vector3(0.0f, 0.84f, 0.52f).normal()*0.2f);
	collisionPlanes[2].setVel(vector3(0.0f, 0.84f, 0.0f).normal()*0.2f);
	stairs.init(); stair1.init(); stair2.init();
	stairs.load(std::ifstream("rsc/EscalatorStairs_Main.txt"));
	stair1.load(std::ifstream("rsc/EscalatorStair.txt"));
	stair2.load(std::ifstream("rsc/EscalatorStair.txt"));
}

void escalator::setPos(const vector3 &p)
{
	pos = p;
	rampPos = pos + vector3(0.0f, 8.0f, 0.0f);
}

void escalator::live()
{
	static float a = 0.0f, b = 0.0f, c = 3.5f;
	a += .2f; if(a > 4.5f) a -= 8.0f;
	b += .2f; if(b > 8.0f) b -= 8.0f;
	stairs.setPos(rampPos + dir * a);
	stair1.setPos(vector3(pos.x, pos.y + b, pos.z));
	stair2.setPos(vector3(pos.x, pos.y + 95.5f + a, pos.z + 52.0f));

	stairs.live();
	stair1.live();
	stair2.live();
}

void escalator::draw(const camera &cam) {
	object::draw(cam);
	stairs.draw(cam);
	stair1.draw(cam);
	stair2.draw(cam);
}

//void escalator::stencilShadow(const point3d &lightPos)
//{
//	object::stencilShadow(lightPos);
//	stairs.stencilShadow(lightPos);
//	stair1.stencilShadow(lightPos);
//	stair2.stencilShadow(lightPos);
//}
