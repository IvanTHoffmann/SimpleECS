#include "SimpleECS/simpleECS.hpp"
#include "appData.hpp"
#include "events.hpp"


void initInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	app->inputManager.windowW = 800;
	app->inputManager.windowH = 800;
}

void updateInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	app->inputManager.update();

	if (glfwWindowShouldClose(appData->window)) {
		app->eventManager.dispatch(&onStop);
	}

	if (app->inputManager.onInputSignal("QUIT")) {
		app->eventManager.dispatch(&onStop);
	}

	if (app->inputManager.onInputSignal("FOCUS")) { // move to ui system
		app->inputManager.windowState ^= WIN_MOUSE_LOCKED; // toggle mouse lock
		app->inputManager.windowState |= WIN_MOUSE_JUMP;
		app->inputManager.mouseInput[2] = 0;
		app->inputManager.mouseInput[3] = 0;
		// GLFW_CURSOR_DISABLED (locked) == GLFW_CURSOR_NORMAL (unlocked) + 2

		if (app->inputManager.windowState & WIN_MOUSE_LOCKED) {
			glfwSetInputMode(appData->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(appData->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}