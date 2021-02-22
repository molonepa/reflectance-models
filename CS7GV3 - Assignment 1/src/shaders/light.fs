#version 400

uniform vec3 u_LightColour;

out vec4 FragColour;

void main() {
	FragColour = vec4(u_LightColour, 1.0);
}