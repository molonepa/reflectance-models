#version 400

in vec3 f_Position;
in vec2 f_UV;
in vec3 f_Normal;

uniform vec3 u_BaseColour;

uniform float u_SpecularThreshold;
uniform float u_BandThreshold;
uniform float u_ShadowThreshold;

uniform vec3 u_LightColour;
uniform vec3 u_LightPosition;
uniform float u_AmbientStrength;

out vec4 FragColour;

void main() {
	float LightIntensity;
	vec3 Result;
	vec3 LightDirection = normalize(u_LightPosition - f_Position);

	LightIntensity = dot(LightDirection, f_Normal);

	if (LightIntensity > u_SpecularThreshold) {
		Result = u_LightColour;
	}
	else if (LightIntensity > u_BandThreshold) {
		Result = u_LightColour * u_BaseColour;
	}
	else if (LightIntensity > u_ShadowThreshold) {
		Result = u_LightColour * 0.5 * u_BaseColour;
	}
	else {
		Result = u_LightColour * 0.25 * u_BaseColour;
	}

	FragColour = vec4(Result, 1.0);
}
