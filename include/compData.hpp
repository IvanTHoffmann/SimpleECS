#pragma once

#include "SimpleECS/util.hpp"

#define COMP_ENUM(a) a ## Enum
#define COMP_BIT(a) a ## Bit
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

// PHYSICS

struct COMP_TYPE(Transform) {
	u8 flags;
	vec3 pos, scale;
	quat rot;
};

struct COMP_TYPE(Rigidbody) {
	u8 flags;
	float invMass;
	mat4 invInertia;
	vec3 vel, lastPos;
	quat spin, lastRot;
};

struct COMP_TYPE(Constraint) {
	u8 flags;
	u32 bodyA, bodyB;
	float compliance;
	TransformComp offsetA;
	TransformComp offsetB;
};

// CONTROL

struct COMP_TYPE(Input) {
	u8 flags;
	u8 controllerId;
	float sensitivity, deceleration;
	float axis[AXIS_COUNT];
	unsigned char button[BUTTON_COUNT]; // TODO: replace with bitmask
	unsigned char hat[HAT_COUNT];
};

struct COMP_TYPE(Character) {
	u8 flags;
	float speed;
};

// GRAPHICS

struct COMP_TYPE(Camera) {
	u8 flags;
	u8 fboId;
	float aspect, fov;
};

struct COMP_TYPE(Mesh) {
	u8 flags;
	GLuint meshId;
	GLuint texId;
	vec2 tiling;
};

#define COMP_CHILD_OFFSET 0x1

struct COMP_TYPE(Child){
	u8 flags;
	u16 parent;
	vec3 offsetPos;
	vec3 offsetRot;
};

// GUI

#define COMP_GUI_PIXEL_POS 0x1
#define COMP_GUI_PIXEL_SCALE 0x2
#define COMP_GUI_PIXEL_ANCHOR 0x4

struct COMP_TYPE(Gui) {
	u8 flags;
	vec3 anchor;
	vec4 color;
};

struct COMP_TYPE(Text) { // 
	u8 flags;
	u8 fontIndex;
	u8 lineCount;
	char str[64]; // TODO: replace with a pointer to memory stored in AppData
	float fontSize;
};

// Audio

#define SOUND_GEN 0x1
#define SOUND_LOOP 0x2

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

// Convenience Variables

#define DECL_TYPE(a) struct COMP_TYPE(a);
FOREACH_COMP(DECL_TYPE)

#define DECL_ENUM(a) COMP_ENUM(a),
enum { FOREACH_COMP(DECL_ENUM) COMP_COUNT };

#define DECL_BIT(a) COMP_BIT(a) = 1<<COMP_ENUM(a),
enum { FOREACH_COMP(DECL_BIT) };

#define GET_SIZE(a) sizeof(a ## Comp),
const u16 compSize[COMP_COUNT] = { FOREACH_COMP(GET_SIZE) };