#include "../ew/external/glad.h"
#include "../ew/camera.h"

struct Light {
	ew::Camera lightCam;
	glm::vec3 lightColor = new glm::vec3(1.0f);

	inline glm::mat4 lightMatrix()
	{
		return lightCam.viewMatrix() * lightCam.projectionMatrix();
	}
};