#version 150

uniform sampler2D textureSampler;

in float flip;
in vec2 uvVar;
in vec2 uvOffset;
out vec4 worldPosition;

out vec4 outColour;

void main()
{
	vec2 finalUV;
	finalUV.x = flip > 0.5f ? uvVar.y : uvVar.x;
	finalUV.y = flip > 0.5f ? 0.5f*uvVar.x : 0.5f*uvVar.y;
	finalUV += uvOffset;
	outColour = texture(textureSampler, finalUV);
}