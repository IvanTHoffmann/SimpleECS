#pragma once

#include "SimpleECS/util.hpp"

#define FOREACH_COMP(f) \
f(Character) \
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

#define COMP_TYPE(a) a ## Comp
#define DECL_TYPE(a) struct COMP_TYPE(a);
FOREACH_COMP(DECL_TYPE)

#define COMP_ENUM(a) a ## Enum
#define DECL_ENUM(a) COMP_ENUM(a),
enum { FOREACH_COMP(DECL_ENUM) COMP_COUNT_ENUM };

#define COMP_BIT(a) a ## Bit
#define DECL_BIT(a) COMP_BIT(a) = 1<<COMP_ENUM(a),
enum { FOREACH_COMP(DECL_BIT) };

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
	u32 parent;
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
	float volume, leftVolume, rightVolume, speed, subIndex, fade;
	u32 sampleIndex;
	u16 soundIndex; // Can refer to a loaded wav file or a sound generator
	u8 flags;
};

struct COMP_TYPE(Listener) {
	u8 flags;
	float volume, focus;
};
