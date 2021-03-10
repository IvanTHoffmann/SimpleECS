#include "SimpleECS/inputManager.hpp"

InputManager::InputManager() {
	windowW = 0;
	windowH = 0;
	windowState = WIN_RESIZED | WIN_MOUSE_JUMP;

	for (u8 i = 0; i < NUM_BINDINGS; i++) {
		actions[i].count = 0;
		actions[i].changed = false;
		actions[i].val = 0.0f;
	}

	for (u8 i = 0; i < KEY_COUNT; i++) {
		keys[i] = 0;
	}

	for (u8 i = 0; i < MAX_MOUSE_INPUTS; i++) {
		mouseInput[i] = 0;
	}
}


bool InputManager::bindInput(u8 func, u8 deviceID, u8 inputID, float antideadzone, float deadzone, float multiplier, float minVal, float maxVal, bool mulDt) {
	if (func >= NUM_BINDINGS) {
		return false;
	}
	if (actions[func].count >= MAX_FUNC_BINDINGS) {
		return false;
	}
	Binding* b = actions[func].bindings + (actions[func].count++);

	b->deviceID = deviceID;
	b->inputID = inputID;
	b->antideadzone = antideadzone;
	b->deadzone = deadzone;
	b->multiplier = multiplier;
	b->minVal = minVal;
	b->maxVal = maxVal;
	b->mulDt = mulDt;

	b->val = 0.0f;
}

void InputManager::setApp(Application* _app) {
	app = _app;
}

float InputManager::getInput(u8 func, float dt) {
	if (func >= NUM_BINDINGS) {
		return 0.0f; // invalid function id
	}
	
	if (actions[func].tested) {
		return actions[func].val; // The action's value has already been calculated this frame. return the stored value
	}

	// The function's value has not been calculated

	float newVal=0, x;
	Binding *b;
	
	for (u8 i = 0; i < actions[func].count; i++) { // loop over all bindings, calculate, and sum their values
		b = actions[func].bindings + i;
		x = b->val;
		x += b->antideadzone * sign(x);
		x = (x - b->deadzone * sign(x)) * (abs(x) > b->deadzone);
		x *= b->multiplier;
		x = CLAMP(x, b->minVal, b->maxVal);
		if (b->mulDt) {
			x *= dt;
		}
		newVal += x;
	}

	actions[func].changed = ((newVal == 0) != (actions[func].val == 0)); // The value changed between 0 and non-0 in the last frame.
	actions[func].val = newVal;
	actions[func].tested = true;
	return newVal;
}

bool InputManager::onInputChanged(u8 func) {
	if (func >= NUM_BINDINGS) {
		return false; // invalid function id
	}
	getInput(func);
	return actions[func].changed;
}

bool InputManager::onInputSignal(u8 func) {
	return getInput(func) && actions[func].changed;
}

bool InputManager::onInputRelease(u8 func) {
	if (func >= NUM_BINDINGS) {
		return false; // invalid function id
	}
	return !getInput(func) && actions[func].changed;
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
	for (u8 i = 0; i < NUM_BINDINGS; i++) {
		for (u8 j = 0; j < actions[i].count; j++) {
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

	for (u8 i = 0; i < NUM_BINDINGS; i++) {
		actions[i].tested = false;
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