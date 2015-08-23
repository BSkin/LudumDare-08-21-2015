#version 150

attribute vec3 vertex;
attribute vec2 uv;

out vec2 uvVar;

void main()
{
	gl_Position = vec4((-2.0f)*vertex.x, 2.0f*vertex.y, 0.0f, 1.0f);
	uvVar = uv;
}