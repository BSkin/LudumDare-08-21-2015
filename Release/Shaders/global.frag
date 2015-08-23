#version 150

uniform sampler2D textureSampler;

in vec2 uvVar;
in vec4 worldPosition;

out vec4 outColour;

void main()
{
	vec2 finalUV = worldPosition.xy * 0.1f;
	outColour = texture(textureSampler, finalUV);
}