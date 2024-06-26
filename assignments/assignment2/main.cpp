#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/shader.h>
#include <ew/model.h>
#include <ew/camera.h>
#include <ew/transform.h>
#include <ew/cameraController.h>
#include <ew/texture.h>
#include <ew/procGen.h>

#include <wr/frameBuffer.h>
#include <wr/light.h>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
GLFWwindow* initWindow(const char* title, int width, int height);
void drawUI();
void resetCamera(ew::Camera* camera, ew::CameraController* controller);

//Global state
int screenWidth = 1080;
int screenHeight = 720;
const int SHADOW_WIDTH = 2048;
const int SHADOW_HEIGHT = 2048;
float prevFrameTime;
float deltaTime;
glm::vec3 lightPos = glm::vec3(0.0, -1.0, 0.0);
glm::vec3 floorPos = glm::vec3(0.0, -1.0, 0.0);

float maxShadowBias = 0.015;
float minShadowBias = 0.005;

wr::FrameBuffer* shadowBuffer;

ew::Camera camera;

ew::CameraController cameraController;
wr::Light light;

ew::Transform monkeyTransform;
ew::Transform floorTransform;

struct Material {
	float Ka = 1.0;
	float Kd = 0.5;
	float Ks = 0.5;
	float Shininess = 128;
}material;

float blurKernel[9]{
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16
};

int main() {
	GLFWwindow* window = initWindow("Assignment 0", screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	wr::FrameBuffer frameBuffer = wr::FrameBuffer(screenWidth, screenHeight, GL_RGB);
	shadowBuffer = new wr::FrameBuffer(SHADOW_WIDTH, SHADOW_HEIGHT, GL_FLOAT, wr::SHADOW, wr::DEPTH16);

	light.orthographic = true;

	ew::Shader shader = ew::Shader("assets/lit.vert", "assets/lit.frag");
	ew::Model monkeyModel = ew::Model("assets/suzanne.obj");
	ew::Mesh planeMesh = ew::Mesh(ew::createPlane(10, 10, 5));
	GLuint brickTexture = ew::loadTexture("assets/brick_color.jpg");

	light.initOrtho(0.1f, 100.0f, 10);
	light.position = lightPos;

	//Binds the shadow map's depth buffer to texture 1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, shadowBuffer->depthBuffer);
	shader.use();
	shader.setInt("_ShadowMap", 1);

	//Bind brick texture to texture unit 0 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);
	//Make "_MainTex" sampler2D sample from the 2D texture bound to unit 0
	shader.use();
	shader.setInt("_MainTex", 0);

	
	camera.position = glm::vec3(0.0f, 0.0f, 5.0f);
	camera.target = glm::vec3(0.0f, 0.0f, 0.0f); //Look at the center of the scene
	camera.aspectRatio = (float)screenWidth / screenHeight;
	camera.fov = 60.0f; //Vertical field of view, in degrees
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Back face culling
	glEnable(GL_DEPTH_TEST); //Depth testing

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		deltaTime = time - prevFrameTime;
		prevFrameTime = time;

		//RENDER
		//glClearColor(0.6f,0.8f,0.92f,1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cameraController.move(window, &camera, deltaTime);
		//light.position = camera.position;
		//light.target = camera.target;

		monkeyTransform.rotation = glm::rotate(monkeyTransform.rotation, deltaTime, glm::vec3(0.0, 1.0, 0.0));
		
		floorTransform.position = floorPos;

		light.position = light.target - light.direction * 5.0f;

		shadowBuffer->UseShadow(light);
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shadowBuffer->FBShader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw(); //Draws monkey model using current shader
		shadowBuffer->FBShader.setMat4("_Model", floorTransform.modelMatrix());
		planeMesh.draw();

		
		
		frameBuffer.UseDefault();
		shader.use();
		shader.setMat4("lightSpaceMatrix", light.lightMatrix());
		shader.setMat4("_ViewProjection", camera.projectionMatrix() * camera.viewMatrix());
		shader.setVec3("_EyePos", camera.position);
		shader.setFloat("_Material.Ka", material.Ka);
		shader.setFloat("_Material.Kd", material.Kd);
		shader.setFloat("_Material.Ks", material.Ks);
		shader.setFloat("_Material.Shininess", material.Shininess);
		shader.setFloat("_MaxShadowBias", maxShadowBias);
		shader.setFloat("_MinShadowBias", minShadowBias);
		shader.setVec3("_LightDirection", light.direction);
		shader.setMat4("_Model", monkeyTransform.modelMatrix());
		monkeyModel.draw(); //Draws monkey model using current shader
		shader.setMat4("_Model", floorTransform.modelMatrix());
		planeMesh.draw();

		//frameBuffer.FBShader.setFloatArray("_Kernel", blurKernel, 9);
		//frameBuffer.FBShader.setFloat("offset", 1.0f / 300.0f);

		frameBuffer.DrawDefault();
		
		drawUI();

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void drawUI() {
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Settings");
	if (ImGui::Button("Reset Camera")) {
		resetCamera(&camera, &cameraController);
	}
	if (ImGui::CollapsingHeader("Material")) {
		ImGui::SliderFloat("AmbientK", &material.Ka, 0.0f, 1.0f);
		ImGui::SliderFloat("DiffuseK", &material.Kd, 0.0f, 1.0f);
		ImGui::SliderFloat("SpecularK", &material.Ks, 0.0f, 1.0f);
		ImGui::SliderFloat("Shininess", &material.Shininess, 2.0f, 1024.0f);
	}

	if (ImGui::CollapsingHeader("Shadows"))
	{
		ImGui::SliderFloat3("Light Direction", &light.direction.x, -2.0f, 2.0f);
		ImGui::SliderFloat("Max Shadow Bias", &maxShadowBias, 0.0f, 0.1f);
		ImGui::SliderFloat("Min Shadow Bias", &minShadowBias, 0.0f, 0.1f);
		ImGui::SliderFloat3("Light Look Target", &light.target.x, -1.0f, 1.0f);
		ImGui::SliderFloat3("Floor Position", &floorPos.x, -10.0f, -1.0f);
	}

	ImGui::End();

	ImGui::Begin("Shadow Map");
	//Using a Child allow to fill all the space of the window.
	ImGui::BeginChild("Shadow Map");
	//Stretch image to be window size
	ImVec2 windowSize = ImGui::GetWindowSize();
	//Invert 0-1 V to flip vertically for ImGui display
	//shadowMap is the texture2D handle
	ImGui::Image((ImTextureID)shadowBuffer->depthBuffer, windowSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::End();


	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	screenWidth = width;
	screenHeight = height;
}

/// <summary>
/// Initializes GLFW, GLAD, and IMGUI
/// </summary>
/// <param name="title">Window title</param>
/// <param name="width">Window width</param>
/// <param name="height">Window height</param>
/// <returns>Returns window handle on success or null on fail</returns>
GLFWwindow* initWindow(const char* title, int width, int height) {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return nullptr;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (window == NULL) {
		printf("GLFW failed to create window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return nullptr;
	}

	//Initialize ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	return window;
}

void resetCamera(ew::Camera* camera, ew::CameraController* controller) {
	camera->position = glm::vec3(0, 0, 5.0f);
	camera->target = glm::vec3(0);
	controller->yaw = controller->pitch = 0;
}
