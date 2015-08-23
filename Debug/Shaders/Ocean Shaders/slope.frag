#version 150

uniform sampler2D textureSampler;
uniform float gridSize;
uniform float lambda;

in vec2 uvVar;

out vec4 outColour;

//(a + ib)(c + id)
//ac + iad + ibc - bd
//(ac - bd) + i(ad + bc)

//need to store i*k*ht
//ht is in .rg

//k is real vec2 (c + i*0), (c + i*0)
//(ac - bd) + i(ad + bc)
//(ac - 0) + i(0 - bc)
//(ac, -bc)

void main()
{
	vec2 ht = texture(textureSampler, uvVar).rg;
	vec2 k = (uvVar * gridSize - vec2(gridSize, gridSize) * 0.5f) * 2.0f * 3.14159265359f / lambda;
	outColour = vec4(vec2(ht.x*k.x, -ht.y*k.x), vec2(ht.x*k.y, -ht.y*k.y));
}