#version 150

uniform mat4 projViewMatrix;
uniform mat4 worldMatrix;

attribute vec3 vertex;
attribute vec2 uv;

out vec2 uvVar;
out vec4 worldPosition;

void main()
{
	uvVar = uv;
	
	worldPosition = worldMatrix * vec4(vertex, 1.0f);
	gl_Position = projViewMatrix * worldPosition;
}