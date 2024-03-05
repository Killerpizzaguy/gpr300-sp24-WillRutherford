#pragma once

#include "../ew/external/glad.h"
#include "../ew/shader.h"
#include "light.h"

namespace wr {
	const enum BufferType
	{
		DEFAULT,
		SHADOW,
		G_BUFFER
	};
	
	const enum DepthType
	{
		DEPTH16 = GL_DEPTH_COMPONENT16,
		DEPTH24 = GL_DEPTH_COMPONENT24,
		DEPTH32 = GL_DEPTH_COMPONENT32,
		DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
	};

	class FrameBuffer {
	public:
		FrameBuffer(unsigned int width, unsigned int height, int colorFormat = GL_RGB, BufferType bufferType = DEFAULT, DepthType depthType = DEPTH24_STENCIL8, bool sampleDepth = false);
		FrameBuffer();
		~FrameBuffer();
		//void Use();
		void UseDefault();
		void UseShadow(Light light);
		void UseGBuffer();
		//void DrawBuffer();
		void DrawDefault();
		void DrawShadow();
		void DrawGBuffer();
		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int colorBuffers[3];
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		ew::Shader FBShader = ew::Shader("assets/BufferShader.vert", "assets/KernelShader.frag");

	private:
		void MakeDefaultBuffer(int colorFormat, bool sampleDepth, DepthType depthType);
		void MakeShadowBuffer(int colorFormat, DepthType depthType);
		void MakeGBuffer();
		
		const int G_BUFFER_FORMATS[3] =
		{
			GL_RGB32F,
			GL_RGB16F,
			GL_RGB16F
		};
		const GLenum drawBuffers[3] = 
		{
			GL_COLOR_ATTACHMENT0, 
			GL_COLOR_ATTACHMENT1, 
			GL_COLOR_ATTACHMENT2
		};

		
		unsigned int bufferVAO;
		BufferType myType;
	};
	//FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
	//void deleteFrameBuffer(FrameBuffer target);
}