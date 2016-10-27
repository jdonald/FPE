#include "common.h"

#include "timer.h"
#include "world.h"
#include "inputRecorder.h"

bool	keys[256];
timer	timer;
world	world;
bool captureMouse = false;
unsigned int frame = 0;

inputRecorder capture;

unsigned int windowWidth = 1024, windowHeight = 768;

void init() {
	for(unsigned int i = 0; i < 256; ++i) keys[i] = false;

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glLineWidth(2);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	world.init();
	world.load(std::ifstream("rsc/world.map"), "rsc/");
	timer.start(20);

	capture.load(std::ifstream("recording.rec"));
	//capture.run(); //auto-play recording
}

void idle() {}

void live() {
	if(keys['w']) world.command(world::FORWARD);
	if(keys['s']) world.command(world::BACKWARD);
	if(keys['a']) world.command(world::STRAFE_LEFT);
	if(keys['d']) world.command(world::STRAFE_RIGHT);
	if(keys[' ']) world.command(world::JUMP);

	world.live();
}

void display() {
	++frame;
	live();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world.draw();
	glutSwapBuffers();

	capture.runFrame(frame);
}

void resizeWindow(int w, int h) {
	glViewport(0, 0, w, h);
	world.updateViewport(w, h);
}

void keyboard(unsigned char ch, int a, int b) {
	if(ch != 'r' && ch != 'p')
		capture.keyboard(frame, ch, false);

	keys[ch] = true;

	switch(ch) {
		case 27: //ESC
			if(captureMouse) {
				captureMouse = false;
				glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
			} else {
				captureMouse = true;
				world.recenterMouse();
				glutSetCursor(GLUT_CURSOR_NONE);
			}
			break;
		case 'b': world.command(world::TOGGLE_SHADER); break;
		case 'e': world.command(world::USE_DOOR); break;
		case '1': world.command(world::CALL_ELEVATOR1); break;
		case '2': world.command(world::CALL_ELEVATOR2); break;
		case '3': world.command(world::CALL_ELEVATOR3); break;
		case 'r':
			if(capture.isRecording())
			{ capture.stop(); capture.save(std::ofstream("recording.rec")); }
			else capture.start();
		break;
		case 'p': frame = 0; capture.run(); break;
		case 'c': world.command(world::TOGGLE_SHADOWS); break;
		case 'o': world.command(world::TOGGLE_OUTLINES); break;
		case 'v': world.command(world::TOGGLE_STEREO); break;
		case 'l': world.command(world::TOGGLE_LIGHTS); break;
	}
}

void keyboardUp(unsigned char ch, int a, int b) { keys[ch] = false; capture.keyboard(frame, ch, true); }
void keyboardSpecial(int k, int a, int b) {}
void keyboardSpecialUp(int k, int a, int b) {}

void mouseClick(int button, int state, int x, int y) {
	capture.mouseClick(frame, button, state == GLUT_UP);

	if(state == GLUT_UP) {
		if(button == GLUT_LEFT_BUTTON) {
			world.command(world::TOGGLE_FLASHLIGHT);
		}
	}
}

void mouseMove(int x, int y) {
	capture.mouseMove(frame, x, y);

	if(captureMouse) world.mouseMove(vector2(x, y));
	else world.mouseHover(vector2(x,y));
}

void mouseDrag(int x, int y) {}

int main(int argc, char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL | GLUT_ACCUM);

	glutInitWindowSize(windowWidth, windowHeight);
	int w = glutGet(GLUT_SCREEN_WIDTH), h = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowPosition((w-windowWidth)/2, (h-windowHeight)/2);
    glutCreateWindow("HW5 - Final Project");

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc(resizeWindow);

	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(keyboardSpecial);
	glutSpecialUpFunc(keyboardSpecialUp);

	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMove);
	glutMotionFunc(mouseDrag);

    init();
	//glutFullScreen(); //fullscreen:on
    glutMainLoop();
    return 0; 
}
