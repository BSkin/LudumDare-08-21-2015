#version 150

uniform sampler2D textureSampler;

in vec2 uvVar;

out vec4 outColour;

void main()
{
	outColour = vec4(texture(textureSampler, uvVar).rgba);
	outColour = vec4(texture(textureSampler, uvVar).rb, 0, 1);
}