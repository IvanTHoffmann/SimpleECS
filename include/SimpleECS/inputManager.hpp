#pragma once

#include "SimpleECS/decl.hpp"
#include "SimpleECS/util.hpp"

#define KEY_COUNT 131

enum WindowStates {
	WIN_MOUSE_LOCKED = 0x1,
	WIN_MOUSE_JUMP = 0x2,
	WIN_RESIZED = 0x4,
};

struct InputDevice {
	float *axes;
	u8 *buttons, *hats;
	int axisCount, buttonCount, hatCount;
};

#define DEVICE_KEYBOARD 16
#define DEVICE_MOUSE 17
#define NUM_MOUSE_AXES 6
#define MAX_MOUSE_INPUTS (NUM_MOUSE_AXES + 20)

enum {
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	LOOK_LEFT,
	LOOK_RIGHT,
	LOOK_UP,
	LOOK_DOWN,
	JUMP,
	QUIT,
	FOCUS,

	NUM_BINDINGS,
};

#define INPUT_AXIS 0
#define INPUT_BUTTON 1
#define INPUT_HAT 2

struct Binding {
	u8 deviceID; // 0-15 gamepad // 16 keyboard // 17 mouse
	u8 inputID; // the index in list of inputs available for the device
	float val; // the raw input value from GLFW
	// filters are applied in the order shown
	float antideadzone; // x += antideadzone * sign(x)
	float deadzone; // x = (x - deadzone * sign(x)) * (abs(x) > deadzone)
	float multiplier; // x *= multiplier
	float minVal, maxVal; // x = CLAMP(x, minVal, maxVal)
	bool mulDt; // multiply this value by dt
};

#define MAX_FUNC_BINDINGS 3 // Each action can only have MAX_FUNC_BINDINGS inputs bound to it

struct Action {
	u8 count;
	bool tested; // true if the value has been tested during the current frame. Used to remove redundantly calculating input
	bool changed; // tells if the value changed between 0 and non-0 since the last frame.
	float val; // the last calculated value
	Binding bindings[MAX_FUNC_BINDINGS];
};

class InputManager {
public:
	Application* app;
	u8 windowState;
	int windowW, windowH;
	float mouseInput[MAX_MOUSE_INPUTS];
	u8 keys[KEY_COUNT];

	Action actions[NUM_BINDINGS];

	InputManager();
	//~InputManager();
	void setApp(Application* _app);

	void update();

	float getInput(u8 inputFunc, float dt = 1.0f);
	bool onInputSignal(u8 inputFunc);
	bool onInputRelease(u8 inputFunc);
	bool onInputChanged(u8 inputFunc);

	bool bindInput(u8 func, u8 deviceID, u8 inputID, float antideadzone = 0.0f, float deadzone = 0.0f, 
		float multiplier = 1.0f, float minVal = -INFINITY, float maxVal = INFINITY, bool mulDt=false);

	void setCallbacks(GLFWwindow* window);

};

void onJoystick(int joyId, int status);

void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

void onMouseButton(GLFWwindow* window, int button, int action, int mods);
void onCursorEnter(GLFWwindow* window, int entered);
void onCursorPos(GLFWwindow* window, double x, double y);
void onScroll(GLFWwindow* window, double x, double y);

void onJoystick(int id, int status); // status = GLFW_CONNECTED / GLFW_DISCONNECTED

void onDrop(GLFWwindow* window, const char** filenames);
void onMonitor(GLFWwindow* window, int id, int status);
void onWindowClose(GLFWwindow* window);
void onWindowContentScale(GLFWwindow* window, float x, float y);
void onWindowFocus(GLFWwindow* window, int focused);
void onWindowIconify(GLFWwindow* window, int iconified);
void onWindowMaximize(GLFWwindow* window, int maximized);
void onWindowPos(GLFWwindow* window, int x, int y);
void onWindowRefresh(GLFWwindow* window);
void onWindowSize(GLFWwindow* window, int w, int h);

void onFramebufferSize(GLFWwindow* window, int w, int h);

enum {
	KEY_SPACE = 0,
	KEY_APOSTROPHE = 7,
	KEY_COMMA = 12,
	KEY_MINUS,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_SEMICOLON = 27,
	KEY_EQUAL = 29,
	KEY_A = 33,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFT_BRACKET,
	KEY_BACKSLASH,
	KEY_RIGHT_BRACKET,
	KEY_GRAVE_ACCENT = 64,

	KEY_WORLD_1,
	KEY_WORLD_2,

	KEY_ESCAPE = 64,
	KEY_ENTER,
	KEY_TAB,
	KEY_BACKSPACE,
	KEY_INSERT,
	KEY_DELETE,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_CAPS_LOCK = 75,
	KEY_SCROLL_LOCK,
	KEY_NUM_LOCK,
	KEY_PRINT_SCREEN,
	KEY_PAUSE,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_F16,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_F21,
	KEY_F22,
	KEY_F23,
	KEY_F24,
	KEY_F25,
	KEY_KP_0 = 105,
	KEY_KP_1,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_4,
	KEY_KP_5,
	KEY_KP_6,
	KEY_KP_7,
	KEY_KP_8,
	KEY_KP_9,
	KEY_KP_DECIMAL,
	KEY_KP_DIVIDE,
	KEY_KP_MULTIPLY,
	KEY_KP_SUBTRACT,
	KEY_KP_ADD,
	KEY_KP_ENTER,
	KEY_KP_EQUAL,
	KEY_LEFT_SHIFT = 122,
	KEY_LEFT_CONTROL,
	KEY_LEFT_ALT,
	KEY_LEFT_SUPER,
	KEY_RIGHT_SHIFT,
	KEY_RIGHT_CONTROL,
	KEY_RIGHT_ALT,
	KEY_RIGHT_SUPER,
	KEY_MENU,
};

enum {MOUSE_X, MOUSE_Y, MOUSE_DX, MOUSE_DY, MOUSE_SCROLL_X, MOUSE_SCROLL_Y};

enum {AXIS_LH, AXIS_LV, AXIS_RH, AXIS_RV, AXIS_LT, AXIS_RT};

#define HAT_N 0b0001
#define HAT_E 0b0010
#define HAT_S 0b0100
#define HAT_W 0b1000
