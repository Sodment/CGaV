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
#include "lights.h"

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
ShaderProgram* spDiffuseOnly;
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
Model* modelShield;
Model* modelTestCube;

float kernel[9] = 
{
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f
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
	//Skybox
	spSkyBox = new ShaderProgram("skybox.vert", NULL, "skybox.frag");
	//Post processing
	spScreenShader = new ShaderProgram("post_processing.vert", NULL, "post_processing.frag");

	//With multiple lights
	spDiffuseOnly = new ShaderProgram("diffuse_only.vert", NULL, "diffuse_only.frag");
	spSimpleTexture = new ShaderProgram("diffuse_specular.vert", NULL, "diffuse_specular.frag");
	spNormalTexture = new ShaderProgram("diff_spec_norm.vert", NULL, "diff_spec_norm.frag");

	//For materials
	spMaterial = new ShaderProgram("material.vert", NULL, "material.frag"); //Has multiple lights
	spSimpleMaterial = new ShaderProgram("simple_material.vert", "simple_material.geom", "simple_material.frag"); //Is only for seeing the model drawn with diffuse

	//Thrash
	spFunnyCat = new ShaderProgram("funnyCat.vert", "funnyCat.geom", "funnyCat.frag");
}

void initModels()
{
	modelBackpack = new Model("res/backpack/backpack.obj");
	modelShield = new Model("res/shield/shield.obj");
	modelTestCube = new Model("res/test_cube/cube.obj");
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
	delete camera, postQuad, modelBackpack;
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

	/*spNormalTexture->use();

	glUniform3fv(spNormalTexture->u("viewPos"), 1, &camera->Position[0]);

	SetDirLight(*spNormalTexture, dirLight);
	SetMulPointLight(*spNormalTexture, pointLights, 2);

	glUniformMatrix4fv(spNormalTexture->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spNormalTexture->u("V"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spNormalTexture->u("M"), 1, false, glm::value_ptr(M));

	modelBackpack->Draw(*spNormalTexture);*/

	/*spDiffuseOnly->use();

	glUniform3fv(spDiffuseOnly->u("viewPos"), 1, &camera->Position[0]);

	SetDirLight(*spDiffuseOnly, dirLight);
	SetMulPointLight(*spDiffuseOnly, pointLights, 2);

	glUniformMatrix4fv(spDiffuseOnly->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spDiffuseOnly->u("V"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spDiffuseOnly->u("M"), 1, false, glm::value_ptr(M));

	modelShield->Draw(*spDiffuseOnly);*/

	spMaterial->use();

	glUniform3fv(spMaterial->u("viewPos"), 1, &camera->Position[0]);

	SetDirLight(*spMaterial, dirLight);
	SetMulPointLight(*spMaterial, pointLights, 2);

	glUniformMatrix4fv(spMaterial->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spMaterial->u("V"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spMaterial->u("M"), 1, false, glm::value_ptr(M));

	modelTestCube->DrawMaterial(*spMaterial);

	/*spSimpleTexture->use();
	glUniform3fv(spSimpleTexture->u("viewPos"), 1, &camera->Position[0]);

	SetDirLight(*spSimpleTexture, dirLight);
	SetMulPointLight(*spSimpleTexture, pointLights, 2);

	glUniformMatrix4fv(spSimpleTexture->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spSimpleTexture->u("V"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spSimpleTexture->u("M"), 1, false, glm::value_ptr(M));

	modelBackpack->Draw(*spSimpleTexture);*/


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
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	spScreenShader->use();
	SetPostPRoccesingKernel(*spScreenShader, kernel);
	postQuad->Draw(*spScreenShader);

	glfwSwapBuffers(window);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		fprintf(stderr, "Cant initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGaV", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Error eh ncreating the window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Cant initialize GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window);
	initShaderPrograms();
	initModels();
	glfwSetTime(0);
	printf("LOOP START");
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}
		freeOpenGLProgram(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
}
