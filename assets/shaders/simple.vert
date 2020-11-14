#version 450

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

out vec3 viewPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

vec4 worldPos;

void main() {
	worldPos = model * vec4(inPos, 1);
	gl_Position = proj * view * worldPos;
	fragNormal = normalize(mat3(transpose(inverse(model))) * inNorm);
	fragTexCoord = inTexCoord;
	fragPos = worldPos.xyz;
	viewPos = (inverse(view) * vec4(0, 0, 0, 1)).xyz;
}