#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "constants.h"
#include "shaderprogram.h"
#include "camera.h"
#include "myCube.h"
#include "model.h"
#include "skybox.h"
#include "post_proccesing.h"

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1024;

float aspectRatio = 1280/1024;
float near_clip = 0.1f;
float far_clip = 1000.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera* camera;
ShaderProgram* spSkyBox;
ShaderProgram* spSimpleTexture;
ShaderProgram* spNormalTexture;
ShaderProgram* spFunnyCat;
ShaderProgram* spMaterial;
ShaderProgram* spSimpleMaterial;
ShaderProgram* spScreenShader;
Model* ourModel;
Model* ourModel2;
SkyBox* skybox;
PostQuad* postQuad;
Model* modelBackpack;
Model* modelCat;

glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  0.0f,  10.0f),
		glm::vec3(0.0f, 0.0f, -10.0f),
};

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) 
	{
		if (key == GLFW_KEY_W) direction.Forward = true;
		if (key == GLFW_KEY_S) direction.Backward = true;
		if (key == GLFW_KEY_A) direction.Left = true;
		if (key == GLFW_KEY_D) direction.Right = true;
		if (key == GLFW_KEY_SPACE) direction.Up = true;
		if (key == GLFW_KEY_LEFT_SHIFT) direction.Down = true;

	}

	if (action == GLFW_RELEASE) 
	{
		if (key == GLFW_KEY_W) direction.Forward = false;
		if (key == GLFW_KEY_S) direction.Backward = false;
		if (key == GLFW_KEY_A) direction.Left = false;
		if (key == GLFW_KEY_D) direction.Right = false;
		if (key == GLFW_KEY_SPACE) direction.Up = false;
		if (key == GLFW_KEY_LEFT_SHIFT) direction.Down = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(yoffset);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}

void initShaderPrograms()
{
	spSkyBox = new ShaderProgram("v_skybox.glsl", NULL, "f_skybox.glsl");
	spSimpleTexture = new ShaderProgram("v_simple_texture.glsl", NULL, "f_simple_texture.glsl");
	spNormalTexture = new ShaderProgram("v_textured_normals.glsl", NULL, "f_textured_normals.glsl");
	spFunnyCat = new ShaderProgram("v_funnyCat.glsl", "g_funnyCat.glsl", "f_funnyCat.glsl");
	spMaterial = new ShaderProgram("v_material.glsl", NULL, "f_material.glsl");
	spSimpleMaterial = new ShaderProgram("v_simple_material.glsl", "g_simple_material.glsl", "f_simple_material.glsl");
	spScreenShader = new ShaderProgram("v_post_processing.glsl", NULL, "f_post_processing.glsl");
}

void initModels()
{
	modelBackpack = new Model("res/backpack/backpack.obj");
	modelCat = new Model("res/cat/cat.obj");
}


//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(1, 0, 1, 1);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	camera = new Camera();
	skybox = new SkyBox();
	postQuad = new PostQuad(SCR_WIDTH, SCR_HEIGHT);

}

void freeOpenGLProgram(GLFWwindow* window) {
	delete camera, postQuad, modelBackpack, modelCat;
	delete spSkyBox,spScreenShader,spFunnyCat,spFunnyCat;
}

void drawScene(GLFWwindow* window) {
	//Binding post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, postQuad->framebuffer);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = camera->GetViewMatrix();

	glm::mat4 P = camera->GetPerspectiveMatrix(aspectRatio, near_clip, far_clip);

	glm::mat4 M = glm::mat4(1.0f);

	spNormalTexture->use();

	glUniform3fv(spNormalTexture->u("viewPos"), 1, &camera->Position[0]);

	glUniform3f(spNormalTexture->u("dirLight.direction"), 0.0f, 100.0f, 0.0f);
	glUniform3f(spNormalTexture->u("dirLight.ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(spNormalTexture->u("dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
	glUniform3f(spNormalTexture->u("dirLight.specular"), 0.5f, 0.5f, 0.5f);

	glUniform3f(spNormalTexture->u("pointLights[0].position"), pointLightPositions[0][0], pointLightPositions[0][1], pointLightPositions[0][2]);
	glUniform3f(spNormalTexture->u("pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(spNormalTexture->u("pointLights[0].diffuse"), 0.0f, 0.8f, 0.8f);
	glUniform3f(spNormalTexture->u("pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(spNormalTexture->u("pointLights[0].constant"), 1.0f);
	glUniform1f(spNormalTexture->u("pointLights[0].linear"), 0.09);
	glUniform1f(spNormalTexture->u("pointLights[0].quadratic"), 0.032);

	glUniform3f(spNormalTexture->u("pointLights[1].position"), pointLightPositions[1][0], pointLightPositions[1][1], pointLightPositions[1][2]);
	glUniform3f(spNormalTexture->u("pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
	glUniform3f(spNormalTexture->u("pointLights[1].diffuse"), 0.8f, 0.0f, 0.8f);
	glUniform3f(spNormalTexture->u("pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
	glUniform1f(spNormalTexture->u("pointLights[1].constant"), 1.0f);
	glUniform1f(spNormalTexture->u("pointLights[1].linear"), 0.09);
	glUniform1f(spNormalTexture->u("pointLights[1].quadratic"), 0.032);

	glUniformMatrix4fv(spNormalTexture->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spNormalTexture->u("V"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spNormalTexture->u("M"), 1, false, glm::value_ptr(M));

	modelBackpack->Draw(*spNormalTexture);


	//Skybox drawing
	V = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	spSkyBox->use();
	glUniformMatrix4fv(spSkyBox->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spSkyBox->u("V"), 1, false, glm::value_ptr(V));
	glDepthMask(GL_FALSE);
	
	skybox->Draw(*spSkyBox);

	glDepthMask(GL_TRUE);


	//Post-processing
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	spScreenShader->use();
	postQuad->Draw(*spScreenShader);

	glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "EXPLOSION", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Nie mo¿na utworzyæ okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Nie mo¿na zainicjowaæ GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window);
	initShaderPrograms();
	initModels();
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		camera->ProcessKeyboard(deltaTime);
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}
		freeOpenGLProgram(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
}
