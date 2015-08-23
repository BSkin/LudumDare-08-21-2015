#version 150

uniform sampler2D textureSampler;

in vec2 uvVar;
out vec4 worldPosition;

out vec4 outColour;

void main()
{
	outColour = texture(textureSampler, uvVar);
}