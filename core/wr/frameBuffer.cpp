#include "frameBuffer.h"
#include<iostream>

wr::FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth, bool hasColor, bool hasDepth, DepthType depthType)
{
	//make the frame buffer
	//wr::FrameBuffer buffer;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//make the color buffer
	if (hasColor)
	{
		glGenTextures(1, &colorBuffer);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	}
	else
	{
		colorBuffer = NULL;
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
		
	
	//make the depth buffer
	if (hasDepth)
	{
		if (sampleDepth) //makes a texture2D if we need to sample depth
		{
			glGenTextures(1, &depthBuffer);
			glBindTexture(GL_TEXTURE_2D, depthBuffer);

			glTexImage2D(GL_TEXTURE_2D, 0, depthType, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
		}
		else //Make a render buffer if we dont need to sample depth
		{
			glGenRenderbuffers(1, &depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

			glRenderbufferStorage(GL_RENDERBUFFER, depthType, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}
	else
		depthBuffer = NULL;

	glCreateVertexArrays(1, &bufferVAO);

	//check if everything works
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame Buffer Created" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

wr::FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fbo);
}

void wr::FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void wr::FrameBuffer::DrawBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	FBShader.use();
	glDisable(GL_DEPTH_TEST);
	glBindTextureUnit(0, colorBuffer);
	glBindVertexArray(bufferVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
}