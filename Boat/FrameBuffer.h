#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <WinSock2.h>
#include <Windows.h>
#include "OpenGL/glew.h"
#include "OpenGL/GLU.h"
#include "OpenGL/GL.h"

#include "Texture.h"
#include "Settings.h"

#include <vector>
using std::vector;

#define MAX_COLOUR_BUFFERS 4

class FrameBuffer
{
public:
	FrameBuffer();
	FrameBuffer(int width, int height);
	~FrameBuffer();
	void addColorTexture();
	void addColorFloatTexture();
	void addDepthTexture();
	void addDepthBuffer();
	GLuint getFrameBufferID() { return frameBufferObject; }
	Texture * getTexture(int x) { if (x < 0 || x >= colourTextures.size()) return NULL; else return colourTextures[x]; }
	Texture * getDepthTexture() { return depthTexture; }
	int getNumColourTexture() { return colourTextures.size(); }
	int getHeight() { return height; }
	int getWidth() { return width; }

	static void setActiveFrameBuffer(FrameBuffer *, GLenum usage = GL_DRAW_FRAMEBUFFER);
private:
	int width, height;
	GLuint frameBufferObject;
	GLuint depthBufferObject;
	Texture * depthTexture;
	vector<Texture*> colourTextures;
};

#endif