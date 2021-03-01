#pragma once

#include "SimpleECS/util.hpp"
#include "portaudio.h"


enum WindowStates {
	WIN_MOUSE_LOCKED = 0x1,
	WIN_MOUSE_JUMP = 0x2,
	WIN_RESIZED = 0x4,
};

struct InputInfo {
	u8 windowState;
	int windowW, windowH;

	double mouseX, mouseY;
	int mouseLB, mouseRB;
	int w, a, s, d, space;
	int mouseDX, mouseDY;
	int scrollX, scrollY;
};

struct PhysicsInfo {
	u8 steps;
	vec3 gravity;
};

struct AudioInfo {
	PaStream* stream;
	PaStreamParameters outputParameters;
	uint16_t numChannels;
	uint32_t sampleRate;
};

struct AppData {
	u8 flags;
	int maxDt;
	GLFWwindow* window;
	InputInfo inputInfo;
	PhysicsInfo physicsInfo;
	AudioInfo audioInfo;

	bool doubleBuf;
	u16 fpsSum;
};