#version 150

uniform mat4 projViewMatrix;
uniform mat4 worldMatrix;
uniform vec2 minUV;
uniform vec2 maxUV;
uniform float flipUV;
uniform vec2 spriteOffset;

attribute vec3 vertex;
attribute vec2 uv;

out vec2 uvOffset;
out float flip;
out vec2 uvVar;
out vec4 worldPosition;

void main()
{
	uvVar.x = uv.x * (maxUV.x - minUV.x) + minUV.x;
	uvVar.y = uv.y * (maxUV.y - minUV.y) + minUV.y;
	flip = flipUV;
	uvOffset = spriteOffset;

	worldPosition = worldMatrix * vec4(vertex, 1.0f);
	gl_Position = projViewMatrix * worldPosition;
}