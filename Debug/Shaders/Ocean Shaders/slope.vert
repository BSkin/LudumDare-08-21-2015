#version 150

uniform float width;
uniform float height;
uniform vec2 position;

attribute vec3 vertex;
attribute vec2 uv;

out vec2 uvVar;

void main()
{
	gl_Position = vec4(position.x - width*vertex.x, position.y + height*vertex.y, 0.0f, 1.0f);
	uvVar = uv;
}