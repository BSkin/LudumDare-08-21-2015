#version 150

uniform sampler2D gaussianSampler;
uniform vec2 windDirection;
uniform float amplitude;
uniform float lambda;
uniform float windSpeed;
uniform float gridSize;

in vec2 uvVar;

out vec4 outColour;

float Ph(vec2 pos)
{
	float L = windSpeed*windSpeed/9.81f;
	float l = L / 10000.0f;
	float k = length(pos);
	return amplitude*exp(-1.0f/pow((k*L),2.0f))/pow(k,4.0f)*pow(abs(dot(pos, windDirection)), 2.0f) * exp(-pow(k,2.0f)*pow(l,2.0f)); //Suppress small waves
}

vec2 h0(vec2 pos)
{
	vec2 outValue;
	vec4 pixel = texture(gaussianSampler, uvVar);
	outValue.x = 1.0f / sqrt(2.0f) * pixel.x * sqrt(Ph(pos)); //real
	outValue.y = 1.0f / sqrt(2.0f) * pixel.y * sqrt(Ph(pos)); //imaginary
	return outValue;
}

void main()
{
	vec2 pos = (uvVar * gridSize - vec2(gridSize, gridSize) * 0.5f) * 2.0f * 3.14159265359f / lambda;
	outColour = vec4(h0(pos), 0.0f, 1.0f);
}