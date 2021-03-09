#pragma once

#include "SimpleECS/decl.hpp"
#include "SimpleECS/util.hpp"

#define KEY_COUNT 131
#define KEY_PRESSED 0b01
#define KEY_CHANGED 0b10

#define updateKey(key, state) (key = ((key&1) != state) << 1 | state)

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

#define DEVICE_KEYBOARD 14
#define DEVICE_MOUSE 15
#define DEVICE_COUNT 16

class InputManager {
public:
	Application* app;
	u8 windowState;
	int windowW, windowH;

	double mouseX, mouseY;
	float mouseDX, mouseDY;
	float scrollX, scrollY;
	u8 mouseLB, mouseRB;
	u8 keys[KEY_COUNT];
	float mouseAxes[6];
	u8 mouseButtons[8];

	InputDevice devices[DEVICE_COUNT]; // 14 gamepads + mouse + keyboard

	InputManager();
	//~InputManager();
	void setApp(Application* _app);

	void update();

	float getAxis(u16 hidMask, u8 axisID);
	u8 getButton(u16 hidMask, u8 buttonID);
	u8 onButtonDown(u16 hidMask, u8 buttonID);
	u8 onButtonUp(u16 hidMask, u8 buttonID);
	u8 getHat(u16 hidMask, u8 hatID);

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

#define CONTROLLER_KEYBOARD 0b00000001
#define CONTROLLER_MOUSE	0b00000010
#define CONTROLLER_JOY0		0b00000100
#define CONTROLLER_JOY1		0b00001000
#define CONTROLLER_JOY2		0b00010000
#define CONTROLLER_JOY3		0b00100000

#define HAT_N 0b0001
#define HAT_E 0b0010
#define HAT_S 0b0100
#define HAT_W 0b1000
