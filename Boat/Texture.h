#ifndef TEXTURE_H
#define TEXTURE_H

#include <WinSock2.h>
#include <Windows.h>

#include "OpenGL/glew.h"
#include "OpenGL/GLU.h"
#include "OpenGL/GL.h"
#include "glm/glm.hpp"

#include "Utilities.h"

#include <string>
#include <stdio.h>

using std::string;
using namespace Utilities;

class Texture
{
public:
	Texture();
	~Texture();
	static Texture * loadTexture(string fileName);
	static Texture * loadBlankTexture(GLint width = 512, GLint height = 512);
	static Texture * loadRandomizedTexture(GLint width = 512, GLint height = 512);
	static Texture * loadBlankFloatTexture(GLint width = 512, GLint height = 512);
	static Texture * loadDepthTexture(GLint width = 512, GLint height = 512);
	static Texture * loadComplexGaussianNumbersTexture(GLint width = 512, GLint height = 512);
	static Texture * loadButterflyLookupTexture(GLint width = 512, GLint height = 512);
	static Texture * loadFFTTest(GLint width = 512, GLint height = 512);
	static Texture * loadiFFTTest(GLint width = 512, GLint height = 512);
	static void saveUncompressedTGA(string filepath, Texture * t);
	void bindTexture() { glBindTexture(GL_TEXTURE_2D, textureID); }
	GLuint getTextureID() { return textureID; } 
	glm::vec4 getPixel(float x, float y);
private:
	static Texture * loadErrorTexture();
	static Texture * loadTGA(FILE * file);
	static Texture * loadCompressedTGA(FILE * file);
	static Texture * loadUncompressedTGA(FILE * file);
	static GLfloat * computeButterflyLookupData(int FFTSize, int numPasses);
	static bool generateTexture(Texture * texture, GLenum fileFormat);

	GLubyte * imageUByteData;
	float * imageFloatData;
	GLenum imageDataType;
	GLuint textureID;
	GLint type;				//GL_RGB or GL_RGBA
	GLint width, height;	//Image Dimensions
	GLint bitdepth;			//24 or 32
};


#endif 