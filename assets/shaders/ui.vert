#version 450

uniform mat4 model;
uniform vec2 framePos;
uniform vec2 frameSize;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

out vec2 fragTexCoord;

void main() {
	gl_Position = model * vec4(inPos, 1);
	fragTexCoord = inTexCoord * frameSize + framePos;
}