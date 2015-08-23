 #version 150

uniform sampler2D FFTSampler;
uniform float scaleFactor;
uniform float textureSize;

in vec2 uvVar;

out vec4 outColour;

//real slopes in r(x), b(z)
//need to convert to vec3() in worldSpace

void main()
{
	vec4 pixel = texture2D(FFTSampler, uvVar);
	bool odd = ((mod((textureSize*(uvVar.x + uvVar.y)), 2.0f)) == 1.0f);
	pixel = odd ? -pixel : pixel;
	outColour = vec4(scaleFactor * pixel);
}