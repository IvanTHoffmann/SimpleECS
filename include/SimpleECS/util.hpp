#pragma once

#include "SimpleECS/dtypes.hpp"

// TODO: Complete these enums for an xinput controller
enum axis_enum { AXIS_LH, AXIS_LV, AXIS_RH, AXIS_RV, AXIS_COUNT };
enum button_enum { BUTTON_0, BUTTON_COUNT };
enum hat_enum { BUTTON_1, HAT_COUNT };

#define GLFW_DLL
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include <bitset> // write custom bitmask class so I can change the size at runtime or use std::vector
#include <map>

#include <iostream>


#define rint(a, b) (rand() % ((b)-(a)) + (a))

#define MILLISECOND ((std::chrono::milliseconds)1)
#define CUR_TIME std::chrono::steady_clock::now()
#define TIME_DIFF(begin, end) (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin))

#define TO_RADS 0.017453292519943295f
#define CLAMP(v, l, h) ((v) > (h) ? (h) : ((v) < (l) ? (l) : (v)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define setMatrix(loc, m) glUniformMatrix4fv((loc), 1, GL_FALSE, value_ptr(m))

#define printVec3(v) printf(#v ": (%f, %f, %f)\n", (v).x, (v).y, (v).z)

using namespace glm;

void printMatrix(mat4 m);

#define INVALID_INDEX_16 ((u16)(-1))
#define INVALID_INDEX ((u32)(-1))

class nameMap {
private:
	std::map<std::string, u16> map;

public:

	std::string getName(u16 index);
	u16 getIndex(std::string name);
	void setIndex(std::string name, u16 index);

	void clear();
	void removeByName(std::string name);
};
