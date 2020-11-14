#version 450

uniform sampler2D tex;
uniform vec4 color;

in vec2 fragTexCoord;

out vec4 pixelColor;

void main() {
	pixelColor = texture(tex, fragTexCoord) * color;
}