#include "object.h"

object::object():pos(0,0,0) {
	yaw = pitch = 0.0f;
}

void object::load(std::istream &in, std::string path) {
	vector3 v;
	char ch;
	std::string filename;

	skipWhitespace(in);
	while(!in.eof() && in.get(ch))
	{
		switch(ch)
		{
		case '/': //comment
			do { in.get(ch); } while(ch != '\n' && !in.eof());
		break;
		case 'f': //file insert
			filename = readToken(in);
			filename = path + filename;
			load(std::ifstream(filename.c_str()), filename.substr(0,filename.find_last_of('/')+1));
		break;
		case 'x': //position
			in >> v;
			setPos(v);
		break;
		case 'M': //model
			mesh.load(in);
		break;
		case 'C': //collision plane
			{
				collisionPlane c;
				c.load(in);
				collisionPlanes.push_back(c);
			}
		break;
		case '}': //end of object
			//updateInvM();
			return;
		break;
		}
		skipWhitespace(in);
	}
}

void object::draw(const camera &cam) {
	glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(yaw, 0.0f, 0.0f, 1.0f);
		glRotatef(pitch, 1.0f, 0.0f, 0.0f);

		currentShaderSet->updateMatrix(cam);
		
		mesh.draw();

		//for(unsigned int i = 0; i < collisionPlanes.size(); ++i)
		//	collisionPlanes[i].draw();

		//glColor3f(1.0f, 1.0f, 1.0f);
		//draw3DBlock(-halfScale+1, -halfScale+1, 0+1, halfScale-1, halfScale-1, scale*2-1);
		//glColor3f(0.0f, 0.0f, 0.0f);
		//draw3DWireframe(-halfScale, -halfScale, 0, halfScale, halfScale, scale);
		//draw3DWireframe(-halfScale, -halfScale, scale, halfScale, halfScale, scale*2);
	glPopMatrix();
}

bool object::clip(character &c) {
	bool clipped = false;
	if(yaw || pitch)
	{
		vector3 s(c.lastPos);			//safe backup
		//VMatMult(invM, &c.lastPos.x);	//transform to local coordinates
		//VMatMult(invM, &c.pos.x);
		c.lastPos = invM * c.lastPos;
		c.pov.pos = invM * c.pov.pos;
		
		for(unsigned int i = 0; i < collisionPlanes.size(); ++i)
			if(collisionPlanes[i].isFloor())
 				clipped |= c.clipAsFloor(collisionPlanes[i], up);
			else if(collisionPlanes[i].isCeiling())
				clipped |= c.clipAsCeiling(collisionPlanes[i], up);
			else
				clipped |= c.clipAsWall(collisionPlanes[i], up);
		
		//VMatMult(mat, &c.pos.x);		//convert to global coordinates
		c.pov.pos = mat * c.pov.pos;
		c.lastPos = s;					//restore backup
	}
	else
	{
		c.lastPos -= pos;	//transform to local coordinates
		c.pov.pos -= pos;
		
		for(unsigned int i = 0; i < collisionPlanes.size(); ++i)
			if(collisionPlanes[i].isFloor())
				clipped |= c.clipAsFloor(collisionPlanes[i]); //will use default up vector (0,0,1)
			else if(collisionPlanes[i].isCeiling())
				clipped |= c.clipAsCeiling(collisionPlanes[i]);
			else
				clipped |= c.clipAsWall(collisionPlanes[i]);
		
		c.pov.pos += pos;		//convert to global coordinates
		c.lastPos += pos;
	}
	return clipped;
}

void object::reCache() { mesh.cache(); }

void object::updateInvM() {
	glPushMatrix();
		glLoadIdentity();
		glRotatef(-pitch, 1.0f, 0.0f, 0.0f);
		glRotatef(-yaw, 0.0f, 0.0f, 1.0f);
		glTranslatef(-pos.x, -pos.y, -pos.z);
		//glGetFloatv(GL_MODELVIEW_MATRIX, invM);
		invM.loadModelView();
	glPopMatrix();

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(yaw, 0.0f, 0.0f, 1.0f);
		glRotatef(pitch, 1.0f, 0.0f, 0.0f);
		//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
		mat.loadModelView();
	glPopMatrix();

	float y = yaw*deg2rad, p = pitch*deg2rad, sp = sinf(p);
	up.x = -sinf(y)*sp;
	up.y = cosf(y)*sp;
	up.z = cosf(p);
}
