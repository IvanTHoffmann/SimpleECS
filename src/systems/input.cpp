#include "SimpleECS/simpleECS.hpp"
#include "appData.hpp"
#include "events.hpp"


void initInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	app->inputManager.windowW = 800;
	app->inputManager.windowH = 800;

	int i;
	for (int joyID = 0; joyID < 4; joyID++) {
		app->inputManager.joysticks[joyID].joyID = -1;
		for (i = 0; i < AXIS_COUNT; i++) {
			app->inputManager.joysticks[joyID].axes[i] = 0;
		}
		for (i = 0; i < BUTTON_COUNT; i++) {
			app->inputManager.joysticks[joyID].buttons[i] = 0;
		}
		for (i = 0; i < HAT_COUNT; i++) {
			app->inputManager.joysticks[joyID].hats[i] = 0;
		}
	}

	for (u8 keyCode = 0; keyCode < KEY_COUNT; keyCode++) {
		app->inputManager.keys[keyCode] = 0;
	}
}

void updateInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	app->inputManager.refreshInput();

	if (glfwWindowShouldClose(appData->window)) {
		app->eventManager.dispatch(&onStop);
	}

	if (app->inputManager.onKeyDown(KEY_DELETE)) {
		app->eventManager.dispatch(&onStop);
	}

	if (app->inputManager.onKeyDown(KEY_ESCAPE)) { // move to ui system
		app->inputManager.windowState ^= WIN_MOUSE_LOCKED; // toggle mouse lock
		app->inputManager.windowState |= WIN_MOUSE_JUMP;
		app->inputManager.mouseDX = 0;
		app->inputManager.mouseDY = 0;
		// GLFW_CURSOR_DISABLED (locked) == GLFW_CURSOR_NORMAL (unlocked) + 2

		if (app->inputManager.windowState & WIN_MOUSE_LOCKED) {
			glfwSetInputMode(appData->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(appData->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	if (app->inputManager.onKeyDown(KEY_J)) { // detect gamepads
		int axisCount;
		for (int joyID = 0; joyID < 16; joyID++) {
			if (glfwGetJoystickAxes(joyID, &axisCount)) {
				app->inputManager.joysticks[0].joyID = joyID;
				break;
			}
		}
	}

	Entity ent(app);

	// update joysticks

	int i, glfwId, axisCount, buttonCount, hatCount;
	float* axes, axis;
	unsigned char* buttons, * hats, button, hat;
	JoystickInfo* joystick;

	for (int joyId = 0; joyId < 4; joyId++) {
		joystick = app->inputManager.joysticks + joyId;
		if (joystick->joyID == -1) {
			continue;
		}
		if (axes = (float*)glfwGetJoystickAxes(joystick->joyID, &axisCount)) {
			for (i = 0; i < AXIS_COUNT && i < axisCount; i++) {
				joystick->axes[i] = axes[i];
			}
		}
		if (buttons = (unsigned char*)glfwGetJoystickButtons(joystick->joyID, &buttonCount)) {
			for (i = 0; i < BUTTON_COUNT && i < buttonCount; i++) {
				joystick->buttons[i] = (((joystick->buttons[i] & 1) != buttons[i]) << 1) | buttons[i];
			}
		}
		if (hats = (unsigned char*)glfwGetJoystickHats(joystick->joyID, &hatCount)) {
			for (i = 0; i < HAT_COUNT && i < hatCount; i++) {
				joystick->hats[i] = (((joystick->hats[i] & 1) != hats[i]) << 1) | hats[i];
			}
		}
	}

	// update entities

	bool foundInput;
	ent.setPrefab(-1);
	while (ent.next(InputBit)) {
		ent.copyInput();
		if (!ent.Input->mask) {
			continue;
		}
		ent.Input->axis[AXIS_LH] = 0;
		ent.Input->axis[AXIS_LV] = 0;
		ent.Input->axis[AXIS_RH] = 0;
		ent.Input->axis[AXIS_RV] = 0;
		ent.Input->button[BUTTON_A] = 0;

		if (ent.Input->mask & CONTROLLER_KEYBOARD) {
			ent.Input->axis[AXIS_LH] += (float)(app->inputManager.isPressed(KEY_D) - app->inputManager.isPressed(KEY_A));
			ent.Input->axis[AXIS_LV] += (float)(app->inputManager.isPressed(KEY_S) - app->inputManager.isPressed(KEY_W));
			//input.axis[AXIS_RH] *= input.deceleration * evnt->dt;
			//input.axis[AXIS_RV] *= input.deceleration * evnt->dt;
			ent.Input->button[BUTTON_A] |= app->inputManager.keys[KEY_SPACE];
		}
		if (ent.Input->mask & CONTROLLER_MOUSE) {
			if (app->inputManager.windowState & (WIN_MOUSE_LOCKED | WIN_MOUSE_JUMP)) {
				ent.Input->axis[AXIS_RH] += app->inputManager.mouseDX * ent.Input->sensitivity;
				ent.Input->axis[AXIS_RV] += app->inputManager.mouseDY * ent.Input->sensitivity;
			}
		}
		if (ent.Input->mask & CONTROLLER_JOY0) {
			ent.Input->axis[AXIS_LH] += app->inputManager.joysticks[0].axes[AXIS_LH] * (abs(app->inputManager.joysticks[0].axes[AXIS_LH]) > .2);
			ent.Input->axis[AXIS_LV] += app->inputManager.joysticks[0].axes[AXIS_LV] * (abs(app->inputManager.joysticks[0].axes[AXIS_LV]) > .2);
			ent.Input->axis[AXIS_RH] += app->inputManager.joysticks[0].axes[AXIS_RH] * ent.Input->sensitivity * 6 * (abs(app->inputManager.joysticks[0].axes[AXIS_RH]) > .2);
			ent.Input->axis[AXIS_RV] += app->inputManager.joysticks[0].axes[AXIS_RV] * ent.Input->sensitivity * 6 * (abs(app->inputManager.joysticks[0].axes[AXIS_RV]) > .2);
			ent.Input->button[BUTTON_A] |= app->inputManager.joysticks[0].buttons[BUTTON_A];

			if (app->inputManager.joysticks[0].buttons[BUTTON_START] == 3) {
				app->eventManager.dispatch(&onStop);
			}
		}
		ent.syncInput();
	}

	for (u8 keyCode = 0; keyCode < KEY_COUNT; keyCode++) {
		app->inputManager.keys[keyCode] &= 1;
	}
}