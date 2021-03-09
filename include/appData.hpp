#pragma once

#include "SimpleECS/util.hpp"
#include "portaudio.h"

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
	PhysicsInfo physicsInfo;
	AudioInfo audioInfo;

	bool doubleBuf;
	u16 fpsSum;
};