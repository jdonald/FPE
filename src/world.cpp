#include "world.h"

bool drawOutlines = false;

world::~world() {
	for(std::vector<object*>::iterator i = objects.begin(); i != objects.end(); ++i)
		delete *i;
	for(std::vector<light*>::iterator i = lights.begin(); i != lights.end(); ++i)
		delete *i;
}

void world::init() {
	ambient.load("rsc/shaders/ambient_vert.cg", "main", "rsc/shaders/ambient_frag.cg", "main");
	shader.load("rsc/shaders/shade_phong_vert.cg", "main", "rsc/shaders/shade_phong_frag.cg", "main");
	emissiveShader.load("rsc/shaders/shade_phong_vert.cg", "main", "rsc/shaders/emissive_frag.cg", "main");
	normShader.load("rsc/shaders/shade_phong_vert.cg", "main", "rsc/shaders/norm_frag.cg", "main");
	shadowShader.load("rsc/shaders/shadow_vert.cg", "main", "rsc/shaders/shadow_frag.cg", "main");
	
	cam.init();
	player.warpTo(vector3(-100,-250,300-30));
	
	flashlight = new spotlight();
	flashlight->isPitchYaw = true;
	flashlight->angle = 22.0f * deg2rad;
	flashlight->clr = vector3(1,1,.8f);
	flashlight->enabled = false;
	lights.push_back(flashlight);
	
	object *o;
	o = new escalator(); objects.push_back(o); o->init();
	o = new door_hinged(); objects.push_back(o); o->init(); theDoor = (door_hinged*)o;
	o = new ::elevator(); objects.push_back(o); o->init(); elevator = (::elevator*)o;

	shader.enable();

	elevator->callToFloor(3);
}

void world::load(std::istream &in, std::string path) {
	char ch;
	object *o;
	light *l;

	skipWhitespace(in);
	while(!in.eof() && in.get(ch))
	{
		switch(ch)
		{
		case '/': //comment
			do { in.get(ch); } while(ch != '\n' && !in.eof());
		break;
		case 'o': //object
			o = new object();
			objects.push_back(o);
			o->load(in, path);
		break;
		case 'l': //light
			if(in.peek() == 's') {
				l = new spotlight();
			} else l = new light();
			lights.push_back(l);
			l->load(in, path);
		break;
		case '}': //end of world
			return;
		break;
		}
		skipWhitespace(in);
	}
}

void world::updateViewport(int w, int h) {
	if(!h) h = 1;
	cam.setAspect(w/(double)h);
	viewportCenter = vector2(w/2, h/2);
}

void world::live() {
	player.liveBegin();
		for(std::vector<object*>::iterator i = objects.begin(); i != objects.end(); ++i)
			{ (*i)->live(); (*i)->clip(player); }
	player.liveEnd();

	cam.setPov(player.getSwayedPov()); // make camera match player perspective (w/sway)

	flashlight->pos = player.pov.pos+vector3(0,0,20); // have flashlight follow player
	static float a = 0.0f, flashYawVel = 0.0f, flashPitchVel = 0.0f;
	a += .01f;
	flashYawVel += (player.pov.yaw+cos(a)*3*sin(a/5)-flashlight->yaw)*.07f;
	flashPitchVel += (player.pov.pitch+sin(a*3)-flashlight->pitch)*.07f;
	flashYawVel *= .75f; flashPitchVel *= .75f;
	flashlight->setPanTilt(flashlight->yaw + flashYawVel, flashlight->pitch + flashPitchVel);

	for(std::vector<light*>::iterator l = lights.begin(); l != lights.end(); ++l) {
		static float a = 0.0f; a += 0.002f;
		if((spotlight*)(*l) && !((spotlight*)(*l))->isPitchYaw) { //animate 'to' based spotlights for extra fun
			//(*l)->pos.x = cosf(a)*100; (*l)->pos.y = sinf(a)*50;
			(*l)->pos.x += cosf(a)*.6f; (*l)->pos.y += sinf(a)*.4f;
		}
	}
}

void world::draw() {
	if(stereoscopic) {
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, (int)viewportCenter.x-5, (int)viewportCenter.y*2);
		cam.leftEye(); renderScene();
		glViewport((int)viewportCenter.x+5, 0, (int)viewportCenter.x-5, (int)viewportCenter.y*2);
		cam.rightEye(); renderScene();
		glViewport(0, 0, (int)viewportCenter.x*2, (int)viewportCenter.y*2);
		cam.center();
	} else {
		switch(depthOfFieldMode) {
			case 0: renderScene(); break;
			case 1:
				glClear(GL_DEPTH_BUFFER_BIT);
				ambient.enable(cam);
				drawGeometry(); //ambient pass (also preps z-buffer for shadow stencil passes)
				for(std::vector<light*>::iterator l = lights.begin(); l != lights.end(); ++l) {
					if(!(*l)->enabled) continue;
					glColor1p((*l)->clr);
					glPushMatrix();
						glTranslate1p((*l)->pos);
						currentShaderSet->updateMatrix(cam);
						glutSolidSphere(2, 10, 10);
					glPopMatrix();
				}		

				float z; //change this to only update the focal distance once total frame
				glReadPixels(cursor.x, cursor.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
				cam.setFocalDistance(min(convertZ(1,1500,z), 500.0f)); //50.0f);

				cam.DoF_1(this, renderScene);
				break;
			case 2:
				cam.DoF_2(this, renderScene); break;
		}
	}
}

void world::renderScene() {
	glClear(GL_DEPTH_BUFFER_BIT);

	//cgShaderSet::disableAll();
	emissiveShader.enable(cam);
	for(std::vector<light*>::iterator l = lights.begin(); l != lights.end(); ++l) {
		if(!(*l)->enabled) continue;
		glColor1p((*l)->clr);
		glPushMatrix();
			glTranslate1p((*l)->pos);
			currentShaderSet->updateMatrix(cam);
			glutSolidSphere(2, 10, 10);
		glPopMatrix();
	}

	ambient.enable(cam);
	drawGeometry(); //ambient pass (also preps z-buffer for shadow stencil passes)

	shader.enable(cam); shadowShader.enable(cam); //prep shaders
	//shader.updateMatrix(cam); shadowShader.updateMatrix(cam); //cant do this, they must be enabled

	glEnable(GL_BLEND); glBlendFunc(GL_ONE, GL_ONE); //blend multi-pass lights
	for(std::vector<light*>::iterator l = lights.begin(); l != lights.end(); ++l) {
		if(!(*l)->enabled) continue; //skip disabled lights
		
		if(shadowsEnabled) stencilShadows(**l);

		//static float a = 0.0f; a += 0.002f;
		//if((spotlight*)(*l) && !((spotlight*)(*l))->isPitchYaw) { //animate 'to' based spotlights for extra fun
		//	//(*l)->pos.x = cosf(a)*100; (*l)->pos.y = sinf(a)*50;
		//	(*l)->pos.x += cosf(a)*.6f; (*l)->pos.y += sinf(a)*.4f;
		//}
		
		shader.enable();
		(*l)->use(); //setup light values in shader

		if(shadowsEnabled) {
			glEnable(GL_STENCIL_TEST); //draw only inside stencil mask
			glDepthFunc(GL_EQUAL);
				drawGeometry();
			glDepthFunc(GL_LEQUAL);
			glDisable(GL_STENCIL_TEST);
		} else drawGeometry();
	}
	glDisable(GL_BLEND);
}

void world::drawGeometry() {
	glBegin(GL_TRIANGLES);
		glNormal1p( (vector3(15, 50, -10)-vector3(-5, 50, -10)).cross( vector3(0, 65, 10)-vector3(-5, 50, -10) ) );
		glVertex3f(-15, 50, 0);
		glVertex3f(5, 50, 0);
		glVertex3f(-10, 65, 20);
	glEnd();

	for(std::vector<object*>::iterator i = objects.begin(); i != objects.end(); ++i)
		(*i)->draw(cam); //draw all registered objects
}

void world::stencilShadows(const light &l) { //mask off light regions (stencil shadow volumes)
	shadowShader.enable(l);
	glColorMask(0, 0, 0, 0); glDepthMask(0); //do not modify buffers
	glClear(GL_STENCIL_BUFFER_BIT); glEnable(GL_STENCIL_TEST); //prep stencil buffer
	glStencilFunc(GL_ALWAYS, 0, ~0);		//set reference stencil value to 0
	glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);	//increment on Z fail
	glCullFace(GL_FRONT); drawGeometry();	//draw back faces only
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);	//decrement on Z fail
	glCullFace(GL_BACK); drawGeometry();	//draw front faces only
	glColorMask(1, 1, 1, 1); glDepthMask(1); glDisable(GL_STENCIL_TEST); //restore normal state
	glStencilFunc(GL_EQUAL, 0, ~0); glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
}

void world::command(commands c) {
	static bool shaderToggle = false;
	switch(c) {
		case FORWARD: player.forwardBy(1.7f); break;
		case BACKWARD: player.forwardBy(-1.7f); break;
		case STRAFE_LEFT: player.strafeBy(-1.0f); break;
		case STRAFE_RIGHT: player.strafeBy(1.0f); break;
		case JUMP: player.accel(vector3(0,0,0.18f)); break;
		case TOGGLE_SHADER: shaderToggle = !shaderToggle; if(shaderToggle) normShader.enable(); else shader.enable(); break;
		case USE_DOOR: theDoor->toggle(); break;
		case TOGGLE_FLASHLIGHT: flashlight->enabled = !flashlight->enabled; break;
		case CALL_ELEVATOR1: elevator->callToFloor(1); break;
		case CALL_ELEVATOR2: elevator->callToFloor(2); break;
		case CALL_ELEVATOR3: elevator->callToFloor(3); break;
		case TOGGLE_SHADOWS: shadowsEnabled = !shadowsEnabled; break;
		case TOGGLE_OUTLINES: drawOutlines = !drawOutlines;
			for(std::vector<object*>::iterator i = objects.begin(); i != objects.end(); ++i)
				(*i)->reCache();
		break;
		case TOGGLE_STEREO: stereoscopic = !stereoscopic; break;
		case TOGGLE_LIGHTS:
			for(std::vector<light*>::iterator i = lights.begin()+2; i != lights.end(); ++i)
				(*i)->enabled = !(*i)->enabled;
		break;
	}
}

void world::mouseMove(vector2 m) {
	static bool warpCatch = false;
	if(warpCatch) { warpCatch = false; return; }

	vector2 diff = m - viewportCenter;
	if(!warpCatch) {
		warpCatch = true;
		recenterMouse();
		player.panTiltBy(diff.x*0.5f, diff.y*0.5f);
	}
}

void world::recenterMouse() {
	glutWarpPointer((int)viewportCenter.x, (int)viewportCenter.y);
	cursor = viewportCenter;
}

void world::mouseHover(vector2 m) {
	cursor.x = m.x;
	cursor.y = viewportCenter.y*2 - m.y;
}
