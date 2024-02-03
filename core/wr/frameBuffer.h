#pragma once

#include "../ew/external/glad.h"

namespace wr {


	struct FrameBuffer {

		unsigned int fbo;
		unsigned int colorBuffer;
		unsigned int depthBuffer;
		unsigned int width;
		unsigned int height;
	};
	FrameBuffer makeFrameBuffer(unsigned int width, unsigned int height, int format, bool sampleDepth = false);
	void deleteFrameBuffer(FrameBuffer target);
}