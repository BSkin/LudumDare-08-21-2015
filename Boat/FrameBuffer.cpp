#include "FrameBuffer.h"

FrameBuffer::FrameBuffer()
{
	frameBufferObject = -1;
	depthBufferObject = -1;
	depthTexture = NULL;

	glGenFramebuffers(1, &frameBufferObject);
}

FrameBuffer::FrameBuffer(int width, int height)
{
	this->width = width;
	this->height = height;

	frameBufferObject = -1;
	depthTexture = NULL;

	glGenFramebuffers(1, &frameBufferObject);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferID());
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 4);
}

FrameBuffer::~FrameBuffer()
{
	delete depthTexture;
	while (colourTextures.size() > 0) {
		delete colourTextures.back();
		colourTextures.pop_back();
	}
	glDeleteFramebuffers(1, &frameBufferObject);
}

void FrameBuffer::addColorTexture(/*Args?*/)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferID());
	Texture * tempTexture = Texture::loadBlankTexture(width, height);
	colourTextures.push_back(tempTexture);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+colourTextures.size()-1, GL_TEXTURE_2D, tempTexture->getTextureID(), 0);
}

void FrameBuffer::addColorFloatTexture(/*Args?*/)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferID());
	Texture * tempTexture = Texture::loadBlankFloatTexture(width, height);
	colourTextures.push_back(tempTexture);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+colourTextures.size()-1, GL_TEXTURE_2D, tempTexture->getTextureID(), 0);
}

void FrameBuffer::addDepthBuffer(/*Args?*/)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferID());
	glGenRenderbuffers (1, &depthBufferObject);
	glBindRenderbuffer (GL_RENDERBUFFER, depthBufferObject); 
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferObject);
}

void FrameBuffer::addDepthTexture(/*Args?*/)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getFrameBufferID());
	depthTexture = Texture::loadDepthTexture(width, height);
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D , depthTexture->getTextureID(), 0);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void FrameBuffer::setActiveFrameBuffer(FrameBuffer * targetFrameBuffer, GLenum usage)
{
	if (targetFrameBuffer == NULL) {
		glBindFramebuffer(usage, 0);
		glViewport(0, 0, Settings::getWindowWidth(), Settings::getWindowHeight());
	}
	else {
		glViewport(0, 0, targetFrameBuffer->getWidth(), targetFrameBuffer->getHeight());
		glBindFramebuffer(usage, targetFrameBuffer->getFrameBufferID());
		GLuint * attachments = (GLuint*)alloca(sizeof(GLuint)*targetFrameBuffer->getNumColourTexture());
		for (int i = 0; i < targetFrameBuffer->getNumColourTexture(); i++)
			attachments[i] = GL_COLOR_ATTACHMENT0 + i;
		if (usage == GL_DRAW_BUFFER)
			glDrawBuffers(targetFrameBuffer->getNumColourTexture(), attachments);
	}
}