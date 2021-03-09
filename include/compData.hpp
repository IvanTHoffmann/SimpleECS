#pragma once

// TODO: generate this file automatically

#include "SimpleECS/util.hpp"

#define COMP_TYPE(a) a ## Comp

#define FOREACH_COMP(f) \
f(Character) \
f(Input) \
f(Camera) \
f(Transform) \
f(Child) \
f(Mesh) \
f(Text) \
f(Gui) \
f(Rigidbody) \
f(Constraint) \
f(Sound) \
f(Listener) \

struct Offset {
	vec3 pos;
	quat rot;
};

// PHYSICS

struct COMP_TYPE(Constraint) {
	u8 flags;
	u8 type;
	u8 numConnections;
	u32 body;
	u32 connections[4]; // constraints can limit bodies or other constraints
	Offset offsets[2];
	float staticFriction;
	float dynamicFriction;
	float restitution;
	float compliance;
};

struct COMP_TYPE(Rigidbody) {
	u8 flags;
	float invMass;
	mat4 invInertia;
	vec3 lastVel, lastPos, vel;
	quat spin, lastRot;
};

struct COMP_TYPE(Transform) {
	u8 flags;
	vec3 pos, scale;
	quat rot;
};

// CONTROL

struct COMP_TYPE(Input) {
	u8 flags;
	u16 HidMask;
	float sensitivity, deceleration;
};

struct COMP_TYPE(Character) {
	u8 flags;
	float speed;
};

// GRAPHICS

struct COMP_TYPE(Camera) {
	u8 flags;
	u16 fboId;
	float aspect, fov;
};

struct COMP_TYPE(Mesh) {
	u8 flags;
	GLuint meshId;
	GLuint diffuseId;
	GLuint normalId;
	GLuint specularId;
	vec2 tiling;
};

struct COMP_TYPE(Child){
	u8 flags;
	u16 parent;
	vec3 offsetPos;
	vec3 offsetRot;
};

// GUI

struct COMP_TYPE(Gui) {
	u8 flags;
	vec3 anchor;
	vec4 color;
};

struct COMP_TYPE(Text) { // 
	u8 flags;
	u8 lineCount;
	u16 fontIndex;
	char str[64]; // TODO: replace with a pointer to memory stored in AppData
	float fontSize;
};

// AUDIO

struct COMP_TYPE(Sound) {
	u8 flags;
	u16 soundIndex; // Can refer to a loaded wav file or a sound generator
	u32 sampleIndex;
	float volume, leftVolume, rightVolume, speed, subIndex, fade;
};

struct COMP_TYPE(Listener) {
	u8 flags;
	float volume, focus;
};
