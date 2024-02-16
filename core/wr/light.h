#pragma once
#include "../ew/external/glad.h"
#include "../ew/camera.h"

namespace wr {
struct Light {
	ew::Camera lightCam;
	glm::vec3 lightColor = glm::vec3(1.0f);

	inline glm::mat4 lightMatrix()
	{
		return lightCam.viewMatrix() * lightCam.projectionMatrix();
	}
};
}
