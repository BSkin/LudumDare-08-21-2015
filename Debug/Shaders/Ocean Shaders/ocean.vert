#version 150

uniform sampler2D heightSampler;

uniform mat4 worldViewProj;
uniform vec3 camPos;
uniform float enableLighting;
uniform vec2 windDirection;

attribute vec3 vertex;

//out float lighting;
out float height;

vec2 worldPosToUV()
{
	return vec2(vertex.x+0.5f, vertex.z+0.5f) / 512.0f;
}

void main()
{
	//lighting = enableLighting;

	vec2 uv = worldPosToUV();
	vec4 pixel = texture(heightSampler, uv);
	height = pixel.r;
	gl_Position = worldViewProj * vec4(vertex.x + windDirection.x*pixel.b, height, vertex.z + windDirection.y*pixel.b, 1.0f);
}