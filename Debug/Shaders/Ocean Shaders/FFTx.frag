#version 150

uniform sampler2D butterflySampler;
uniform sampler2D FFTSampler;
uniform float butterflyPass;

in vec2 uvVar;

out vec4 outColour;

void main()
{
	vec4 data = texture(butterflySampler, vec2(uvVar.x, butterflyPass));
	vec2 indices = data.rg;
	vec2 weights = data.ba;

	vec4 input1 = texture(FFTSampler, vec2(indices.x, uvVar.y)).rgba;
	vec4 input2 = texture(FFTSampler, vec2(indices.y, uvVar.y)).rgba;

	vec4 weighted2;
	weighted2.r = weights.r * input2.r - weights.g * input2.g;
	weighted2.g = weights.g * input2.r + weights.r * input2.g;

	weighted2.b = weights.r * input2.b - weights.g * input2.a;
	weighted2.a = weights.g * input2.b + weights.r * input2.a;

	outColour = input1 + weighted2;
}