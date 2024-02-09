#pragma once

#include "../ew/external/glad.h"
#include "../ew/shader.h"

namespace wr {
	const enum DepthType
	{
		DEPTH16 = GL_DEPTH_COMPONENT16,
		DEPTH24 = GL_DEPTH_COMPONENT24,
		DEPTH32 = GL_DEPTH_COMPONENT32,
		DEPTH24_STENCIL8 = GL_DEPTH24_STENCIL8,
	};

	class FrameBuffer {
	public:
		FrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false, bool hasColor = true, bool hasDepth = true, DepthType depthType = DEPTH24_STENCIL8);
		~FrameBuffer();
		void Use();
		void DrawBuffer();
		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
		ew::Shader FBShader = ew::Shader("assets/BufferShader.vert", "assets/KernelShader.frag");

	private:
		unsigned int bufferVAO;
	};
	//FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
	//void deleteFrameBuffer(FrameBuffer target);
}