#pragma once

#include "../ew/external/glad.h"
#include "../ew/shader.h"

namespace wr {


	class FrameBuffer {
	public:
		FrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
		~FrameBuffer();
		void Use();
		void DrawBuffer();
		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;

	private:
		unsigned int bufferVAO;
		ew::Shader FBShader = ew::Shader("assets/BufferShader.vert", "assets/BufferShader.frag");
	};
	//FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
	//void deleteFrameBuffer(FrameBuffer target);
}