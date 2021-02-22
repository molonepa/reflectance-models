#version 400

in vec3 f_Position;
in vec2 f_UV;
in vec3 f_Normal;

uniform vec3 u_BaseColour;

uniform vec3 u_LightColour;
uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;

uniform float u_AmbientStrength;
uniform float u_Shininess;
uniform float u_SpecularReflectivity;

out vec4 FragColour;

void main() {
	vec3 Result = vec3(0.0);

	vec3 LightDirection = normalize(u_LightPosition - f_Position);
	vec3 ViewDirection = normalize(u_CameraPosition - f_Position);
	vec3 HalfwayDirection = normalize(LightDirection + ViewDirection);

	vec3 AmbientComponent = u_AmbientStrength * u_LightColour;
	vec3 DiffuseComponent = max(dot(f_Normal, LightDirection), 0.0) * u_LightColour;
	vec3 SpecularComponent = u_SpecularReflectivity * pow(max(dot(f_Normal, HalfwayDirection), 0.0), u_Shininess) * u_LightColour;

	Result = (AmbientComponent + DiffuseComponent + SpecularComponent) * u_BaseColour;
	FragColour = vec4(Result, 1.0);
}