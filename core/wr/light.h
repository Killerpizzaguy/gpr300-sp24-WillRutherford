#pragma once
#include "../ew/external/glad.h"
#include "../ew/camera.h"

const int COLOR_PRESET_COUNT = 6;
const glm::vec3 RED = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 ORANGE = glm::vec3(1.0f, 0.5f, 0.0f);
const glm::vec3 YELLOW = glm::vec3(1.0f, 1.0f, 0.0f);
const glm::vec3 GREEN = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 BLUE = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 PURPLE = glm::vec3(0.5f, 0.0f, 1.0f);

namespace wr {
	struct Light : ew::Camera {
		//ew::Camera lightCam;
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

		inline glm::mat4 lightMatrix()
		{
			return  projectionMatrix() * viewMatrix();
		}

		void initOrtho(float nearPlane, float farPlane, float orthoHeight)
		{
			orthographic = true;
			this->nearPlane = nearPlane;
			this->farPlane = farPlane;
			this->orthoHeight = orthoHeight;
			aspectRatio = 1;
			target = glm::vec3(0.0f);
		}
	};

	struct PointLight {

		glm::vec3 position;
		float radius;
		glm::vec4 color;
	};
}
