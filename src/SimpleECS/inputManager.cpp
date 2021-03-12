#include "SimpleECS/inputManager.hpp"

InputManager::InputManager() {
	windowW = 0;
	windowH = 0;
	windowState = WIN_RESIZED | WIN_MOUSE_JUMP;

	for (u8 i = 0; i < NUM_ACTIONS; i++) {
		actions[i].countAndFlags = 0;
		actions[i].val = 0.0f;
	}

	for (u8 i = 0; i < KEY_COUNT; i++) {
		keys[i] = 0;
	}

	for (u8 i = 0; i < MAX_MOUSE_INPUTS; i++) {
		mouseInput[i] = 0;
	}
}


bool InputManager::bindInput(u8 action, u8 deviceID, u8 inputID, float deadzone, float inMul, float outMul, bool mulDt) {
	if (action >= NUM_ACTIONS) {
		return false;
	}
	if (ACTION_COUNT(actions[action]) >= MAX_BINDINGS_PER_ACTION) {
		return false;
	}
	Binding* b = actions[action].bindings + ACTION_COUNT(actions[action]);
	actions[action].countAndFlags += (1 << 2);

	b->deviceID = deviceID;
	b->inputID = inputID;
	b->deadzone = deadzone;
	b->inMul = inMul;
	b->outMul = outMul / (1 + deadzone);
	b->mulDt = mulDt;

	b->val = 0.0f;
}

void InputManager::setApp(Application* _app) {
	app = _app;
}

float InputManager::getInput(u8 action, float dt) {
	if (action >= NUM_ACTIONS) {
		return 0.0f; // invalid function id
	}
	
	if (ACTION_TESTED(actions[action])) {
		return actions[action].val; // The action's value has already been calculated this frame. return the stored value
	}

	// The function's value has not been calculated

	float newVal=0, x;
	Binding *b;
	
	for (u8 i = 0; i < ACTION_COUNT(actions[action]); i++) { // loop over all bindings, calculate, and sum their values
		b = actions[action].bindings + i;
		x = b->val * b->inMul - b->deadzone;
		if (x <= 0) { continue; }
		x *= b->outMul;
		if (b->mulDt) { x *= dt; }
		newVal += x;
	}

	actions[action].countAndFlags |= ((newVal == 0) != (actions[action].val == 0));  // The value changed between 0 and non-0 in the last frame.
	actions[action].countAndFlags |= TESTED_FLAG;
	actions[action].val = newVal;
	return newVal;
}

bool InputManager::onInputChanged(u8 action) {
	if (action >= NUM_ACTIONS) {
		return false; // invalid function id
	}
	getInput(action);
	return ACTION_CHANGED(actions[action]);
}

bool InputManager::onInputSignal(u8 action) {
	return getInput(action) && ACTION_CHANGED(actions[action]);
}

bool InputManager::onInputRelease(u8 action) {
	if (action >= NUM_ACTIONS) {
		return false; // invalid function id
	}
	return !getInput(action) && ACTION_CHANGED(actions[action]);
}


void InputManager::setCallbacks(GLFWwindow* window) {
	glfwSetWindowUserPointer(window, this);

	// KEYBOARD
	glfwSetKeyCallback(window, onKey);

	// MOUSE
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetCursorEnterCallback(window, onCursorEnter);
	glfwSetCursorPosCallback(window, onCursorPos);
	glfwSetScrollCallback(window, onScroll);

	// JOYSTICK
	glfwSetJoystickCallback(onJoystick);

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

void InputManager::update() { 
	windowState &= ~WIN_RESIZED;

	int inputCount;
	float* axes;
	unsigned char *buttons, *hats;

	Binding* b;
	u8 inputID;

	// loop over inputs and update binding values
	for (u8 i = 0; i < NUM_ACTIONS; i++) {
		for (u8 j = 0; j < ACTION_COUNT(actions[i]); j++) {
			b = actions[i].bindings + j;
			switch (b->deviceID) {
			case DEVICE_KEYBOARD:
				if (b->inputID < KEY_COUNT) {
					b->val = keys[b->inputID];
				}
				break;
			case DEVICE_MOUSE:
				// TODO: process mouse data
				if (b->inputID < MAX_MOUSE_INPUTS) {
					b->val = mouseInput[b->inputID];
				}

				break;
			default:
				if (b->deviceID < DEVICE_KEYBOARD) {
					// joystick
					inputID = b->inputID;
					axes = (float*)glfwGetJoystickAxes(b->deviceID, &inputCount);
					if (inputCount > inputID) {
						b->val = axes[inputID];
					}
					inputID -= inputCount;
					buttons = (u8*)glfwGetJoystickButtons(b->deviceID, &inputCount);
					if (inputCount > inputID) {
						b->val = buttons[inputID];
					}
					inputID -= inputCount;
					hats = (u8*)glfwGetJoystickHats(b->deviceID, &inputCount); // consider processing hat data into 8 buttons
					if (inputCount > inputID) {
						b->val = hats[inputID];
					}
				}
				break;
			}
		}
	}

	for (u8 i = 2; i < 6; i++) { // clear mouse velocity and scrolling
		mouseInput[i] = 0;
	}

	for (u8 i = 0; i < NUM_ACTIONS; i++) {
		actions[i].countAndFlags &= 0b11111100;
	}

	glfwPollEvents();
}

/*
void MessageCallback(source, msg_type, msg_id, severity, length, message, userParam) {
	printf("GL CALLBACK: {} type = {}, severity = {}, message = {}\n",
		   source, msg_type, severity, message);
}
*/

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	//printf("onKey: scancode: %d, key: %d, action: %d, mods: %d\n", scancode, key, action, mods);
	if (action < 2 && ((GLFW_KEY_SPACE <= key && key <= GLFW_KEY_GRAVE_ACCENT) || (GLFW_KEY_ESCAPE <= key && key <= GLFW_KEY_MENU))) {
		u8 keyCode = key & GLFW_KEY_ESCAPE ? key - GLFW_KEY_ESCAPE + 64 : key - GLFW_KEY_SPACE;
		inp->keys[keyCode] = action;
		cout << "key pressed: " << (int)inp->keys[keyCode] << " (" << (int)keyCode << ")\n";
	}
}

void onCursorPos(GLFWwindow* window, double x, double y) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	inp->mouseInput[2] = (x - inp->mouseInput[0]) * !(inp->windowState & WIN_MOUSE_JUMP);
	inp->mouseInput[3] = (y - inp->mouseInput[1]) * !(inp->windowState & WIN_MOUSE_JUMP);
	inp->windowState &= ~WIN_MOUSE_JUMP;
	inp->mouseInput[0] = x;
	inp->mouseInput[1] = y;
}

void onCursorEnter(GLFWwindow* window, int entered) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	if (entered) {
		inp->windowState |= WIN_MOUSE_JUMP;
	}
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	u8 inputID = NUM_MOUSE_AXES + button;
	if (inputID < MAX_MOUSE_INPUTS) {
		inp->mouseInput[inputID] = action;
	}
}

void onScroll(GLFWwindow* window, double x, double y) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	inp->mouseInput[4] = x;
	inp->mouseInput[5] = y;
	printf("scroll: (%f, %f)\n", x, y);
}

void onWindowSize(GLFWwindow* window, int w, int h) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	inp->windowState |= WIN_RESIZED;
	inp->windowW = w;
	inp->windowH = h;
	printf("window resized: (%d, %d)\n", w, h);
}

void onJoystick(int joyId, int status) {
	int axisCount, buttonCount, hatCount;
	glfwGetJoystickAxes(joyId, &axisCount);
	glfwGetJoystickButtons(joyId, &buttonCount);
	glfwGetJoystickHats(joyId, &hatCount);
	cout << "joystick #" << joyId << ' ' << ((status == GLFW_CONNECTED) ? "connected" : "disconnected") <<
		"( axes: " << axisCount << ", buttons: " << buttonCount << ", hats: " << hatCount << ")\n";
}