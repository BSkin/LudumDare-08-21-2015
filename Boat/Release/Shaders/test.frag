#version 150

uniform sampler2D texS;

in float t;
in float lighting;
in vec2 uvVar;
in vec3 normalVar;

out vec4 outColour;

void main()
{
	vec3 sunDir = normalize(vec3(0.1, 0.5, 0.5));
	float intensity = min(1, max(dot(sunDir, normalVar), 0) + 0.2f);

	//outColour = texture(texS, uvVar).rgba;
	//outColour = vec4(intensity*texture(texS, uvVar).rgb, texture(texS, uvVar).a);
	outColour = ((lighting >= 0.5f) ? vec4(intensity*texture(texS, uvVar).rgb, texture(texS, uvVar).a) : texture(texS, uvVar).rgba);
}