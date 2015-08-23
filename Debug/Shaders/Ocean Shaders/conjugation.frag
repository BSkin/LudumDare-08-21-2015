#version 150

uniform sampler2D textureSampler;

in vec2 uvVar;

out vec4 outColour;

void main()
{
	vec2 pixel = texture(textureSampler, uvVar).xy;
	outColour = vec4(pixel.x, -pixel.y, 0.0f, 1.0f);
}