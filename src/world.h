#pragma once

extern bool drawOutlines;

#include <vector>
#include "shaders.h"
#include "camera.h"
#include "character.h"
#include "model.h"
#include "light.h"

#include "object.h"
#include "objectClasses/escalator.h"
#include "objectClasses/door_hinged.h"
#include "objectClasses/elevator.h"

class world {
	int depthOfFieldMode;

	vector2 viewportCenter, cursor;
	cgShaderSet ambient, shader, normShader, emissiveShader;
	cgShadowShaderSet shadowShader;
	camera cam;
	character player;
	spotlight *flashlight;
	std::vector<object*> objects;
	std::vector<light*> lights;

	door_hinged *theDoor;
	elevator *elevator;

	bool shadowsEnabled, stereoscopic;
	void renderScene();

public:
	world() { shadowsEnabled = stereoscopic = false; depthOfFieldMode = 1; }
	~world();
	void init();
	void load(std::istream &, std::string path);
	void updateViewport(int w, int h);
	void live();
	void draw();
	void drawGeometry();
	void stencilShadows(const light &);

	enum commands {
		FORWARD, BACKWARD, STRAFE_LEFT, STRAFE_RIGHT,
		JUMP, TOGGLE_SHADER, USE_DOOR, TOGGLE_FLASHLIGHT,
		CALL_ELEVATOR1, CALL_ELEVATOR2, CALL_ELEVATOR3,
		TOGGLE_SHADOWS, TOGGLE_OUTLINES, TOGGLE_STEREO, TOGGLE_LIGHTS
	};
	void command(commands);
	void mouseMove(vector2);
	void recenterMouse();
	void mouseHover(vector2);

	static void renderScene(void* w) { ((world*)w)->renderScene(); }
};