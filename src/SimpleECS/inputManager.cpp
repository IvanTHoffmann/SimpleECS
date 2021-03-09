#include "SimpleECS/inputManager.hpp"

InputManager::InputManager() {
	mouseX = 0;
	mouseY = 0;
	mouseDX = 0;
	mouseDY = 0;
	scrollX = 0;
	scrollY = 0;
	mouseLB = 0;
	mouseRB = 0;
	windowW = 0;
	windowH = 0;
	windowState = WIN_RESIZED | WIN_MOUSE_JUMP;

	for (u8 i = 0; i < DEVICE_COUNT; i++) {
		devices[i].axisCount = 0;
		devices[i].buttonCount = 0;
		devices[i].hatCount = 0;
	}

	devices[DEVICE_KEYBOARD].buttonCount = KEY_COUNT;
	devices[DEVICE_KEYBOARD].buttons = keys;

	devices[DEVICE_MOUSE].buttonCount = 8;
	devices[DEVICE_MOUSE].buttons = mouseButtons;
	devices[DEVICE_MOUSE].axisCount = 6;
	devices[DEVICE_MOUSE].axes = mouseAxes;

}

void InputManager::setApp(Application* _app) {
	app = _app;
}

float InputManager::getAxis(u16 hidMask, u8 axisID) {
	float value = 0;
	for (u8 i = 0; i < DEVICE_COUNT; i++) {
		if (hidMask & (1 << i) && axisID < devices[i].axisCount) {
			value += devices[i].axes[axisID];
		}
	}
	return value;
}

u8 InputManager::getButton(u16 hidMask, u8 buttonID) {
	u8 value = 0;
	for (u8 i = 0; i < DEVICE_COUNT; i++) {
		if (hidMask & (1 << i) && buttonID < devices[i].buttonCount) {
			value |= devices[i].buttons[buttonID];
		}
	}
	return value;
}

u8 InputManager::onButtonDown(u16 hidMask, u8 buttonID) {
	return getButton(hidMask, buttonID) == 3;
}

u8 InputManager::onButtonUp(u16 hidMask, u8 buttonID) {
	return getButton(hidMask, buttonID) == 2;
}

u8 InputManager::getHat(u16 hidMask, u8 hatID) {
	unsigned char value = 0;
	for (u8 i = 0; i < DEVICE_COUNT; i++) {
		if (hidMask & (1 << i) && hatID < devices[i].hatCount) {
			value |= devices[i].hats[hatID];
		}
	}
	return value;
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

	int axisCount, buttonCount, hatCount;
	float *axes;
	unsigned char i, *buttons, *hats;
	InputDevice *device;

	for (int inputID = 0; inputID < DEVICE_COUNT - 2; inputID++) {
		device = devices + inputID;

		// TODO: Copy data to allocated array and add keydown/keyup flags

		device->axes = (float*)glfwGetJoystickAxes(inputID, &device->axisCount);
		device->buttons = (u8*)glfwGetJoystickButtons(inputID, &device->buttonCount);
		device->hats = (u8*)glfwGetJoystickHats(inputID, &device->hatCount);
	}

	devices[DEVICE_MOUSE].axes[MOUSE_DX] = 0;
	devices[DEVICE_MOUSE].axes[MOUSE_DY] = 0;
	devices[DEVICE_MOUSE].axes[MOUSE_SCROLL_X] = 0;
	devices[DEVICE_MOUSE].axes[MOUSE_SCROLL_Y] = 0;

	for (u8 keyCode = 0; keyCode < KEY_COUNT; keyCode++) {
		devices[DEVICE_KEYBOARD].buttons[keyCode] &= 1;
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
		inp->devices[DEVICE_KEYBOARD].buttons[keyCode] = action + KEY_CHANGED;
		cout << "key pressed: " << (int)inp->devices[DEVICE_KEYBOARD].buttons[keyCode] << " (" << (int)keyCode << ")\n";
	}
}

void onCursorPos(GLFWwindow* window, double x, double y) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	inp->devices[DEVICE_MOUSE].axes[2] = (x - inp->devices[DEVICE_MOUSE].axes[0]) * !(inp->windowState & WIN_MOUSE_JUMP);
	inp->devices[DEVICE_MOUSE].axes[3] = (y - inp->devices[DEVICE_MOUSE].axes[1]) * !(inp->windowState & WIN_MOUSE_JUMP);
	inp->windowState &= ~WIN_MOUSE_JUMP;
	inp->devices[DEVICE_MOUSE].axes[0] = x;
	inp->devices[DEVICE_MOUSE].axes[1] = y;
}

void onCursorEnter(GLFWwindow* window, int entered) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	if (entered) {
		inp->windowState |= WIN_MOUSE_JUMP;
	}
}

void onMouseButton(GLFWwindow* window, int button, int action, int mods) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	updateKey(inp->devices[DEVICE_MOUSE].buttons[button], action);
}

void onScroll(GLFWwindow* window, double x, double y) {
	InputManager* inp = (InputManager*)glfwGetWindowUserPointer(window);
	inp->devices[DEVICE_MOUSE].axes[MOUSE_SCROLL_X] = x;
	inp->devices[DEVICE_MOUSE].axes[MOUSE_SCROLL_Y] = y;
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