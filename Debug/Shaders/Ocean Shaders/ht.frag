#version 150

uniform sampler2D h0Sampler;
uniform float lambda;
uniform float gridSize;
uniform float time;

in vec2 uvVar;

out vec4 outColour;

vec2 ht(vec2 a, vec2 b)
{
	//float k = length((uvVar * gridSize - vec2(gridSize, gridSize) * 0.5f) * 2.0f * 3.14159265359f / lambda);
	float k = length(gridSize*(uvVar - vec2(0.5f, 0.5f)) * 2.0f * 3.14159265359f / lambda);
	float wt = sqrt(9.81f * k) * time * 0.001f;
	float c = cos(wt);
	float s = sin(wt);
	//return vec2(a.x*c - a.y*s + b.x*c + b.y*s, -(a.y*c + a.x*s + b.y*c - b.x*s));
	return vec2((a.x + b.x)*c + (-a.y + b.y)*s, -((a.y + b.y)*c + (a.x - b.x)*s));
	//return vec2((a.x + b.x)*c + (-a.y + b.y)*s, (a.y + b.y)*c + (a.x - b.x)*s);
}

vec2 dtx(vec2 htResult)
{
	vec2 normalK = normalize(uvVar - vec2(0.5f, 0.5f));
	//return vec2(normalK.y, -normalK.x)*htResult;
	return vec2(normalK.y*htResult.x - (-normalK.x)*htResult.y, normalK.y*htResult.y + (-normalK.x)*htResult.x); 
}

void main()
{
	vec2 pos1 = uvVar;
	vec2 pos2 = vec2(1.0f, 1.0f) - pos1;
	vec2 input1 = texture(h0Sampler, pos1).xy;
	vec2 input2 = texture(h0Sampler, pos2).xy;
	vec2 htResult = ht(input1, input2);
	outColour = vec4(htResult, dtx(htResult));
}