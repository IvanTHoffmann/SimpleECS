#pragma once

#include "SimpleECS/dtypes.hpp"

// 32-96
// 256-348

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

// 69

#define CONTROLLER_KEYBOARD 0b00000001
#define CONTROLLER_MOUSE	0b00000010
#define CONTROLLER_JOY0		0b00000100
#define CONTROLLER_JOY1		0b00001000
#define CONTROLLER_JOY2		0b00010000
#define CONTROLLER_JOY3		0b00100000

#define HAT_COUNT 1
enum axis_enum { AXIS_LH, AXIS_LV, AXIS_RH, AXIS_RV, AXIS_LT, AXIS_RT, AXIS_COUNT };
enum button_enum {
	BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y, BUTTON_LB, BUTTON_RB, BUTTON_SELECT, BUTTON_START,
	BUTTON_LS, BUTTON_RS, BUTTON_UP, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_LEFT, BUTTON_COUNT
};

#define HAT_N 0b0001
#define HAT_E 0b0010
#define HAT_S 0b0100
#define HAT_W 0b1000


#define GLFW_DLL
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include <bitset> // write custom bitmask class so I can change the size at runtime or use std::vector
#include <map>

#include <stdio.h>
#include <iostream>


#define rint(a, b) (rand() % ((b)-(a)) + (a))

#define MILLISECOND ((std::chrono::milliseconds)1)
#define CUR_TIME std::chrono::steady_clock::now()
#define TIME_DIFF(begin, end) (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin))

#define TO_RADS 0.017453292519943295f
#define CLAMP(v, l, h) ((v) > (h) ? (h) : ((v) < (l) ? (l) : (v)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define setMatrix(loc, m) glUniformMatrix4fv((loc), 1, GL_FALSE, value_ptr(m))

#define printVec3(v) printf(#v ": (%f, %f, %f)\n", (v).x, (v).y, (v).z)

using namespace glm;
using namespace std;

void printMatrix(mat4 m);

#define INVALID_INDEX_16 ((u16)(-1))
#define INVALID_INDEX ((u32)(-1))

class NameMap {
private:
	std::map<std::string, u16> StrToID;
	std::map<u16, std::string> IDToStr;
public:
	bool getName(std::string* name, u16 index);
	bool getIndex(u16* index, std::string name);
	void add(std::string name, u16 index);
	void remove(std::string name);
	void remove(u16 id);
	void clear();
};

#define PRINT_VAR(v, t) printf(#v ": %" #t "\n", v);
#define CMP0(v) (((v) > 0 ? ((v) != 0) : -((v) != 0)))
#define MOD1(f) ((f) - (int)(f))
#define LERP(a, b, l) ((a) * (l) + (b) * (1 - (l)))