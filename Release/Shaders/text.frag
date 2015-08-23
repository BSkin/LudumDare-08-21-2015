#version 150

uniform vec3 textColour;
out vec4 outColour;

void main()
{
	outColour = vec4(textColour, 1.0f);
}