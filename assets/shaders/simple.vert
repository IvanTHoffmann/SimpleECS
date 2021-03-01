#version 450

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float time;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inTexCoord;

out mat3 tanToModel;
out vec3 viewPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;

vec4 worldPos;

void main() {
	worldPos = model * vec4(inPos, 1);
	fragPos = worldPos.xyz;
	gl_Position = proj * view * worldPos;
	fragNormal = normalize(mat3(transpose(inverse(model))) * inNorm);
	//viewPos = (inverse(view) * vec4(0, 0, 0, 1)).xyz;
    viewPos = vec3(inverse(view)[3]);
	fragTexCoord = inTexCoord;

    tanToModel[2] = fragNormal.xyz;
    tanToModel[1] = normalize(vec3(1, 1, 1));
    tanToModel[0] = cross(tanToModel[1].xyz, tanToModel[2].xyz);
    tanToModel[1] = cross(tanToModel[0].xyz, tanToModel[2].xyz);
}