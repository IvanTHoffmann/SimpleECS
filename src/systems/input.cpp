#include "systems/input.hpp"

#include "events.hpp"


void inputInit(InputInfo *inp) {
	inp->w = 0;
	inp->a = 0;
	inp->s = 0;
	inp->d = 0;
	inp->mouseX = 0;
	inp->mouseY = 0;
	inp->mouseDX = 0; 
	inp->mouseDY = 0;
	inp->scrollX = 0;
	inp->scrollY = 0;
	inp->mouseLB = 0;
	inp->mouseRB = 0;
	inp->windowW = 0;
	inp->windowH = 0;
	inp->windowState = WIN_RESIZED | WIN_MOUSE_JUMP;
}

void refreshInput(InputInfo *inputInfo) {
	inputInfo->windowState &= ~WIN_RESIZED;
	inputInfo->mouseDX = 0;
	inputInfo->mouseDY = 0;
	inputInfo->scrollX = 0;
	inputInfo->scrollY = 0;
	glfwPollEvents();
}

void setCallbacks(GLFWwindow *window, InputInfo *inputInfo) {
	glfwSetWindowUserPointer(window, inputInfo);

	setMouseLock(inputInfo->windowState & WIN_MOUSE_LOCKED);

	// KEYBOARD
	glfwSetKeyCallback(window, onKey);

	// MOUSE
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetCursorEnterCallback(window, onCursorEnter);
	glfwSetCursorPosCallback(window, onCursorPos);
	glfwSetScrollCallback(window, onScroll);

	// JOYSTICK
	//glfwSetJoystickCallback(window, onJoystick);

	// WINDOW
	//glfwSetDropCallback(window, onDrop);
	//glfwSetMonitorCallback(window, onMonitor);
	//glfwSetWindowCloseCallback(window, onWindowClose);
	//glfwSetWindowContentScaleCallback(window, onWindowContentScale);
	//glfwSetWindowFocusCallback(window, onWindowFocus);
	//glfwSetWindowIconifyCallback(window, onWindowIconify);
	//glfwSetWindowMaximizeCallback(window, onWindowMaximize);
	//glfwSetWindowPosCallback(window, onWindowPos);
	//glfwSetWindowRefreshCallback(window, onWindowRefresh);
	glfwSetWindowSizeCallback(window, onWindowSize);
	//glfwSetFramebufferSizeCallback(window, onFramebufferSize);
	//
}

/*
void MessageCallback(source, msg_type, msg_id, severity, length, message, userParam) {
	printf("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
		   source, msg_type, severity, message);
}
*/

void onKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
	//printf("onKey: scancode: %d, key: %d, action: %d, mods: %d\n", scancode, key, action, mods);

	if (action < 2) {
		InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);

		switch (key) {
			case GLFW_KEY_W:
				inputInfo->w = action;
				break;
			case GLFW_KEY_A:
				inputInfo->a = action;
				break;
			case GLFW_KEY_S:
				inputInfo->s = action;
				break;
			case GLFW_KEY_D:
				inputInfo->d = action;
				break;
			case GLFW_KEY_ESCAPE:
				if (action == 1) {
					inputInfo->windowState ^= WIN_MOUSE_LOCKED; // toggle mouse lock
					inputInfo->windowState |= WIN_MOUSE_JUMP;
					inputInfo->mouseDX = 0;
					inputInfo->mouseDY = 0;
					// GLFW_CURSOR_DISABLED (locked) == GLFW_CURSOR_NORMAL (unlocked) + 2
					setMouseLock(inputInfo->windowState & WIN_MOUSE_LOCKED);
					break;
				}
		}
	}
}

void onCursorPos(GLFWwindow *window, double x, double y) {
	InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);
	inputInfo->mouseDX = (x - inputInfo->mouseX) * !(inputInfo->windowState & WIN_MOUSE_JUMP);
	inputInfo->mouseDY = (y - inputInfo->mouseY) * !(inputInfo->windowState & WIN_MOUSE_JUMP);
	inputInfo->windowState &= ~WIN_MOUSE_JUMP;
	inputInfo->mouseX = x;
	inputInfo->mouseY = y;
}

void onCursorEnter(GLFWwindow *window, int entered) {
	if (entered) {
		InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);
		inputInfo->windowState |= WIN_MOUSE_JUMP;
	}
}

void onMouseButton(GLFWwindow *window, int button, int action, int mods) {
	printf("onMouseButton: button: %d, action: %d, mods: %d\n", button, action, mods);
	InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);

	switch (button) {
		case 0:
			inputInfo->mouseLB = (action == 1);
			break;
		case 1:
			inputInfo->mouseRB = (action == 1);
			break;
	}
}

void onScroll(GLFWwindow *window, double x, double y) {
	InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);

	inputInfo->scrollX += x;
	inputInfo->scrollY += y;
	printf("scroll: (%f, %f)\n", x, y);
}

void onWindowSize(GLFWwindow *window, int w, int h) {
	InputInfo *inputInfo = (InputInfo*)glfwGetWindowUserPointer(window);

	inputInfo->windowState |= WIN_RESIZED;
	inputInfo->windowW = w;
	inputInfo->windowH = h;
	printf("window resized: (%d, %d)\n", w, h);
}


void updateJoysticks() {
	int id = 0, i, axisCount, buttonCount, hatCount;
	float *axes;
	unsigned char *buttons, *hats;

	if (!(axes = (float*)glfwGetJoystickAxes(id, &axisCount))) {
		//continue
		return;
	}
	buttons = (unsigned char*)glfwGetJoystickButtons(id, &buttonCount);
	hats = (unsigned char*)glfwGetJoystickHats(id, &hatCount);

	for (i = 0; i < axisCount; i++) {
		printf("axis   %2d: %d\n", i, axes[i]);
	}
	for (i = 0; i < buttonCount; i++) {
		printf("button %2d: %d\n", i, buttons[i]);
	}
	for (i = 0; i < hatCount; i++) {
		printf("hat    %2d: %d\n", i, hats[i]);
	}
}

void initInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	inputInit(&appData->inputInfo);
	appData->inputInfo.windowW = 800;
	appData->inputInfo.windowH = 800;

}

void updateInputSystem(CB_PARAMS) {
	AppData* appData = (AppData*)app->getData();
	refreshInput(&appData->inputInfo);

	if (glfwWindowShouldClose(appData->window)) {
		app->eventManager.dispatch(&onStop);
	}

	Entity ent(app);

	while (ent.next(InputBit)) {
		ent.copyInput();
		if (ent.Input->controllerId == 0) {
			if (appData->inputInfo.windowState & (WIN_MOUSE_LOCKED | WIN_MOUSE_JUMP)) {
				ent.Input->axis[AXIS_LH] = (appData->inputInfo.d - appData->inputInfo.a);
				ent.Input->axis[AXIS_LV] = (appData->inputInfo.s - appData->inputInfo.w);
				//input.axis[AXIS_RH] *= input.deceleration * evnt->dt;
				//input.axis[AXIS_RV] *= input.deceleration * evnt->dt;
				ent.Input->axis[AXIS_RH] = appData->inputInfo.mouseDX * ent.Input->sensitivity;
				ent.Input->axis[AXIS_RV] = appData->inputInfo.mouseDY * ent.Input->sensitivity;
				ent.syncInput();
			}
		}
	}
}