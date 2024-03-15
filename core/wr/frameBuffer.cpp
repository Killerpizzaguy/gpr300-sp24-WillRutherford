#include "frameBuffer.h"
#include<iostream>

wr::FrameBuffer::FrameBuffer()
{

}

wr::FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, int colorFormat, BufferType bufferType, DepthType depthType, bool sampleDepth)
{
	//make the frame buffer
	//wr::FrameBuffer buffer;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	this->width = width;
	this->height = height;
	myType = bufferType;

	switch (bufferType)
	{
	case wr::DEFAULT:
		MakeDefaultBuffer(colorFormat, sampleDepth, depthType);
		break;
	case wr::SHADOW:
		MakeShadowBuffer(colorFormat, depthType);
		break;
	case wr::G_BUFFER:
		MakeGBuffer();
		break;
	default:
		break;
	}

	glCreateVertexArrays(1, &bufferVAO);

	//check if everything works
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame Buffer Created" << std::endl;
		
	}
	else
	{
		std::cout << "Frame Buffer Creation Failed" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void wr::FrameBuffer::MakeDefaultBuffer(int colorFormat, bool sampleDepth, DepthType depthType)
{
	//make the color buffer
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, colorFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	//make the depth buffer
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

void wr::FrameBuffer::MakeShadowBuffer(int colorFormat, DepthType depthType)
{
	FBShader = ew::Shader("assets/ShadowBuffer.vert", "assets/ShadowBuffer.frag");
	colorBuffer = NULL;

	//make the depth buffer
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);

	//glTexImage2D(GL_TEXTURE_2D, 0, depthType, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void wr::FrameBuffer::MakeGBuffer()
{
	FBShader = ew::Shader("assets/geoPass.vert", "assets/geoPass.frag");
	for (size_t i = 0; i < 3; i++)
	{
		glGenTextures(1, &colorBuffers[i]);
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, G_BUFFER_FORMATS[i], width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorBuffers[i], 0);
	}
	glDrawBuffers(3, drawBuffers);

	//Depth Texture
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
}

wr::FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fbo);
}

void wr::FrameBuffer::UseDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void wr::FrameBuffer::UseShadow(Light light)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT);
	FBShader.use();
	FBShader.setMat4("_ViewProjection", light.lightMatrix());
}

void wr::FrameBuffer::UseGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	FBShader.use();
}

void wr::FrameBuffer::DrawDefault()
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

void wr::FrameBuffer::DrawShadow()
{
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*ConfigureShaderAndMatrices();
	glBindTexture(GL_TEXTURE_2D, depthMap);
	RenderScene();*/
}

void wr::FrameBuffer::DrawGBuffer()
{

}