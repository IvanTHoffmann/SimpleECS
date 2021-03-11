#pragma once

#include "SimpleECS/dtypes.hpp"

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

#define MOD1(f) ((f) - (int)(f))
#define LERP(a, b, l) ((a) * (l) + (b) * (1 - (l)))