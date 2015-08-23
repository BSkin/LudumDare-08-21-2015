#version 150

//in float lighting;
in float height;

out vec4 outColour;

void main()
{
	//vec3 normalVar = vec3(0.0f, 1.0f, 0.0f);
	//vec3 sunDir = normalize(vec3(0.1, 0.5, 0.5));
	//float intensity = min(1, max(dot(sunDir, normalVar), 0) + 0.2f);

	//outColour = ((lighting >= 0.5f) ? vec4(intensity*vec3(0.0f, 0.0f, 0.5f + (height + 3.0f) / 6.0f), 1.0f) : vec4(0.0f, 0.0f, 0.5f + height/3.0f, 1.0f));
	outColour = vec4(0.0f, 0.0f, 0.5f + height/3.0f, 1.0f);
}