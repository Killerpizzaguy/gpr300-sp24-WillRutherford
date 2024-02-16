#pragma once

#include "../ew/external/glad.h"
#include "../ew/shader.h"
#include "light.h"

namespace wr {
	const enum BufferType
	{
		DEFAULT,
		SHADOW
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
		FrameBuffer(unsigned int width = 1080, unsigned int height = 720, int colorFormat = GL_RGB, BufferType bufferType = DEFAULT, DepthType depthType = DEPTH24_STENCIL8, bool sampleDepth = false);
		~FrameBuffer();
		//void Use();
		void UseDefault();
		void UseShadow(Light light);
		//void DrawBuffer();
		void DrawDefault();
		void DrawShadow();
		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		ew::Shader FBShader = ew::Shader("assets/BufferShader.vert", "assets/KernelShader.frag");

	private:
		void MakeDefaultBuffer(int colorFormat, bool sampleDepth, DepthType depthType);
		void MakeShadowBuffer(int colorFormat, DepthType depthType);
		
		
		
		
		unsigned int bufferVAO;
		BufferType myType;
	};
	//FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
	//void deleteFrameBuffer(FrameBuffer target);
}