#include "frameBuffer.h"
#include<iostream>

wr::FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false)
{
	//make the frame buffer
	wr::FrameBuffer buffer;
	glGenFramebuffers(1, &buffer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo);

	//make the color buffer
	glGenTextures(1, &buffer.colorBuffer);
	glBindTexture(GL_TEXTURE_2D, buffer.colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer.colorBuffer, 0);
	
	//make the depth buffer
	if (sampleDepth) //makes a texture2D if we need to sample depth
	{
		glGenTextures(1, &buffer.depthBuffer);
		glBindTexture(GL_TEXTURE_2D, buffer.depthBuffer);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, buffer.depthBuffer, 0);
	}
	else //Make a render buffer if we dont need to sample depth
	{
		glGenRenderbuffers(1, &buffer.depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, buffer.depthBuffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.depthBuffer);
	}

	//check if everything works
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame Buffer Created" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, buffer.fbo); //not sure if this is needed here or somewhere else
	}


	return buffer;
}

void deleteFrameBuffer(wr::FrameBuffer target)
{
	glDeleteFramebuffers(1, &target.fbo);
}

