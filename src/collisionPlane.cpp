#include "collisionPlane.h"

collisionPlane::collisionPlane():pos(0,0,0), vel(0,0,0), vertexLists(1)
{
	dynamic = false;
	floor = ceiling = false;
}

void collisionPlane::load(std::istream &in)
{
	dynamic = true;
	bool keepDynamic = false;
	char ch;
	vector2 v;
	float a, b;
	skipWhitespace(in);
	while(!in.eof() && in.get(ch))
	{
		switch(ch)
		{
		case '/': //comment
			do { in.get(ch); } while(ch != '\n' && !in.eof());
		break;
		case 'f': floor = true; break;			//floor
		case 'c': ceiling = true; break;		//ceiling
		case 'd': keepDynamic = true; break;	//dynamic
		case 'x': in >> pos; break;				//position
		case 'u':								//up unit vector (orientation)
			in >> up; up.normalize();
		break;
		case 'r':								//right unit vector (orientation)
			in >> right; right.normalize();
		break;
		case 'v': in >> v; addVertex(v); break;	//vertex 2D
		case 'm': in >> a >> b; addMargin(a, b); break; //margin (extend clipping margin)
		case 'h': addHole(); break;				//hole
		case '}':								//end of object
			refactor();
			if(!keepDynamic) { originals.clear(); dynamic = false; }
			return;
		break;
		}
		skipWhitespace(in);
	}
}

void collisionPlane::setUpAndRight(const vector3 &u, const vector3 &r)
{
	up = u; right = r;
	if(dynamic) refactor();
}

void collisionPlane::addVertex(const vector2 &p)
{
	if(dynamic)
	{
		if(originals.size() == 0) originals.resize(1);
		originals[originals.size()-1].push_back(p);
	}
	vertexLists[vertexLists.size()-1].push_back(right*p.x + up*p.y);
	if(vertexLists[0].size() == 3) plane.calcPlane(vertexLists[0][0], vertexLists[0][1], vertexLists[0][2]);
}

void collisionPlane::addVertex(const vector3 &p)
{
	vertexLists[vertexLists.size()-1].push_back(p);
	if(vertexLists[0].size() == 3) plane.calcPlane(vertexLists[0][0], vertexLists[0][1], vertexLists[0][2]);
}

void collisionPlane::addMargin(float x, float y)
{
	if(dynamic) originals[originals.size()-1][originals[originals.size()-1].size()-1] += vector2(x*COLLISION_PLANE_BUFFER, y*COLLISION_PLANE_BUFFER);
	vertexLists[vertexLists.size()-1][vertexLists[vertexLists.size()-1].size()-1] += right*x*COLLISION_PLANE_BUFFER + up*y*COLLISION_PLANE_BUFFER;
}

void collisionPlane::addHole()
{
	if(dynamic) originals.resize(originals.size()+1);
	vertexLists.resize(vertexLists.size()+1);
}

void collisionPlane::live()
{
	pos += vel;
}

void collisionPlane::draw() const
{
	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glNormal1p(plane.normal);
		//glColor4f(0.5f, 0.5f, 0.0f, 0.5f);
		//glBegin(GL_POLYGON);
		//	for(unsigned int i = 0; i < vertexLists[0].size(); ++i)	
		//		glVertex1p(vertexLists[0][i]);
		//glEnd();
		glColor4f(0.5f, 0.5f, 0.0f, 1.0f);
		for(unsigned int l = 0; l < vertexLists.size(); ++l)
		{
			glBegin(GL_LINE_LOOP);
				for(unsigned int i = 0; i < vertexLists[l].size(); ++i)	
					glVertex1p(vertexLists[l][i]);
			glEnd();
		}
	glPopMatrix();
}

bool collisionPlane::clip(const vector3 &p1, vector3 &p2, float threshold) const //from, to, buffer
{
	float d = plane.distance(p1-pos+vel);
	if(d >= 0.0f)
	{
		d = plane.distance(p2-pos);
		if(d < threshold)
		{
			vector3 s = p2 - plane.normal*d;
			if(!insidePoly(0, s-pos)) return false;
			for(unsigned int h = 1; h < vertexLists.size(); ++h)
				if(insidePoly(h, s-pos)) return false;
			p2 = s + plane.normal*threshold;
			return true;
		}
	}
	return false;
}

bool collisionPlane::clip(const vector3 &p1, vector3 &p2, float threshold, vector3 &v) const //from, to, buffer, vel of plane if isFloor
{
	if(clip(p1, p2, threshold)) { v = vel; return true; }
	return false;
}

bool collisionPlane::clip(const vector3 &p1, vector3 &p2, float threshold, vector3 &v, const vector3 &up) const //from, to, buffer, vel of plane if isFloor
{
	float d = plane.distance(p1-pos+vel);
	if(d >= 0.0f)
	{
		vector3 p = p2 - pos; //p2 localized
		d = plane.distance(p);
		if(d < threshold)
		{
			vector3 s = p - plane.normal*d;
			if(!insidePoly(0, s)) return false;
			for(unsigned int h = 1; h < vertexLists.size(); ++h)
				if(insidePoly(h, s)) return false;
			float f = (threshold - p.dot(plane.normal) - plane.dNormal) / up.dot(plane.normal);
			p2 += up*f; //dont slide (move p2 straight up to resolve)
			if(floor) v = vel;
			return true;
		}
	}
	return false;
}

bool collisionPlane::insidePoly(unsigned int n, const vector3 &p) const
{
	vector3 current, last = p - vertexLists[n][vertexLists[n].size()-1];
	float sum = 0.0f;
	for(unsigned int i = 0; i < vertexLists[n].size(); ++i)
	{
		current = p-vertexLists[n][i];
		sum += current.angleBetween(last);
		last = current;
	}
	return fabs(sum - 2*M_PI) < 0.0001f;
}

void collisionPlane::refactor()
{
	if(!originals.size()) return;
	for(unsigned int i = 0; i < originals.size(); ++i)
		for(unsigned int ii = 0; ii < originals[i].size(); ++ii)
			vertexLists[i][ii] = right*originals[i][ii].x + up*originals[i][ii].y;
	plane.calcPlane(vertexLists[0][0], vertexLists[0][1], vertexLists[0][2]);
}
