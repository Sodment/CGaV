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
#include "lodepng.h"
#include "shaderprogram.h"
#include "camera.h"
#include "read_textures.h"
#include "myCube.h"
#include "model.h"
#include "skybox.h"

float aspectRatio = 1;
float near_clip = 0.1f;
float far_clip = 1000.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera* camera;
ShaderProgram* sp;
ShaderProgram* spSkyBox;
ShaderProgram* spSimpleTexture;
ShaderProgram* spNormalTexture;
ShaderProgram* spFunnyCat;
ShaderProgram* spMaterial;
ShaderProgram* spSimpleMaterial;
Model* ourModel;
Model* ourModel2;
Model* ourModel3;
SkyBox* skybox;


float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
float* c1 = myCubeC1;
float* c2 = myCubeC2;
float* c3 = myCubeC3;
int vertexCount = myCubeVertexCount;
int choice = 0;

glm::vec3 pointLightPositions[] = {
		glm::vec3(0.0f,  0.0f,  10.0f),
		glm::vec3(0.0f, 0.0f, -10.0f),
};

GLuint tex0;
GLuint tex1;
GLuint tex2;
GLuint tex3;
GLuint skyBox;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) direction.Forward = true;
		if (key == GLFW_KEY_S) direction.Backward = true;
		if (key == GLFW_KEY_A) direction.Left = true;
		if (key == GLFW_KEY_D) direction.Right = true;
		if (key == GLFW_KEY_F) choice = 0;
		if (key == GLFW_KEY_G) choice = 1;
		if (key == GLFW_KEY_H) choice = 2;
		if (key == GLFW_KEY_J) choice = 3;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) direction.Forward = false;
		if (key == GLFW_KEY_S) direction.Backward = false;
		if (key == GLFW_KEY_A) direction.Left = false;
		if (key == GLFW_KEY_D) direction.Right = false;
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
	glViewport(0, 0, width, height);
}


//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(1, 0, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	camera = new Camera();
	sp = new ShaderProgram("v_lab8.glsl", NULL, "f_lab8.glsl");
	spSkyBox = new ShaderProgram("v_skybox.glsl", NULL, "f_skybox.glsl");
	spSimpleTexture = new ShaderProgram("v_simple_texture.glsl", NULL, "f_simple_texture.glsl");
	spNormalTexture = new ShaderProgram("v_textured_normals.glsl", NULL, "f_textured_normals.glsl");
	spFunnyCat = new ShaderProgram("v_funnyCat.glsl", "g_funnyCat.glsl", "f_funnyCat.glsl");
	spMaterial = new ShaderProgram("v_material.glsl", NULL, "f_material.glsl");
	spSimpleMaterial = new ShaderProgram("v_simple_material.glsl","g_simple_material.glsl", "f_simple_material.glsl");
	ourModel = new Model("res/backpack/backpack.obj");
	ourModel2 = new Model("res/cat/cat.obj");
	ourModel3 = new Model("res/Models/test.obj");
	skybox = new SkyBox();
}

void freeOpenGLProgram(GLFWwindow* window) {
	delete camera;
	delete spSkyBox;
	delete sp;
}

float amount;
//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = camera->GetViewMatrix();

	glm::mat4 P = camera->GetPerspectiveMatrix(aspectRatio, near_clip, far_clip);

	glm::mat4 M = glm::mat4(1.0f);

	switch (choice)
	{
		case 0:
		{
			spSimpleMaterial->use();
			glUniformMatrix4fv(spSimpleMaterial->u("P"), 1, false, glm::value_ptr(P));
			glUniformMatrix4fv(spSimpleMaterial->u("V"), 1, false, glm::value_ptr(V));

			M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));
			glUniformMatrix4fv(spSimpleMaterial->u("M"), 1, false, glm::value_ptr(M));
			glUniform3fv(spSimpleMaterial->u("diffuse"), 1, &ourModel->meshes[0].material.Diffuse[0]);
			ourModel->Draw(*spSimpleMaterial);

			M = glm::translate(M, glm::vec3(0.0f, 5.0f, 0.0f));
			M = glm::rotate(M, PI / 2, glm::vec3(-1.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));

			glUniformMatrix4fv(spSimpleMaterial->u("M"), 1, false, glm::value_ptr(M));
			glUniform3fv(spSimpleMaterial->u("diffuse"), 1, &ourModel2->meshes[0].material.Diffuse[0]);
			ourModel->Draw(*spSimpleMaterial);
			break;
		}
		case 1:
		{
			spMaterial->use();
			glUniformMatrix4fv(spMaterial->u("P"), 1, false, glm::value_ptr(P));
			glUniformMatrix4fv(spMaterial->u("V"), 1, false, glm::value_ptr(V));

			glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f) * glm::vec3(0.5f);
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

			glUniform3f(spMaterial->u("light.position"), 0.0f, 7.0f, 5.0f);
			glUniform3fv(spMaterial->u("viewPos"), 1, &camera->Position[0]);

			glUniform3fv(spMaterial->u("light.ambient"), 1, &ambientColor[0]);
			glUniform3fv(spMaterial->u("light.diffuse"), 1, &diffuseColor[0]);
			glUniform3f(spMaterial->u("light.diffuse"), 1.0f, 1.0f, 1.0f);

			glUniform3fv(spMaterial->u("material.ambient"), 1, &ourModel3->meshes[0].material.Ambient[0]);
			glUniform3fv(spMaterial->u("material.diffuse"), 1, &ourModel3->meshes[0].material.Diffuse[0]);
			glUniform3fv(spMaterial->u("material.specular"), 1, &ourModel3->meshes[0].material.Specular[0]);
			glUniform1f(spMaterial->u("material.shininess"), ourModel3->meshes[0].material.Shininess);

			M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

			glUniformMatrix4fv(spMaterial->u("M"), 1, false, glm::value_ptr(M));

			ourModel3->Draw(*spMaterial);

			M = glm::translate(M, glm::vec3(0.0f, 5.0f, 0.0f));
			M = glm::rotate(M, PI / 2, glm::vec3(-1.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));

			glUniform3fv(spMaterial->u("material.ambient"), 1, &ourModel2->meshes[0].material.Ambient[0]);
			glUniform3fv(spMaterial->u("material.diffuse"), 1, &ourModel2->meshes[0].material.Diffuse[0]);
			glUniform3fv(spMaterial->u("material.specular"), 1, &ourModel2->meshes[0].material.Specular[0]);
			glUniform1f(spMaterial->u("material.shininess"), ourModel2->meshes[0].material.Shininess);

			glUniformMatrix4fv(spMaterial->u("M"), 1, false, glm::value_ptr(M));

			ourModel2->Draw(*spMaterial);
			break;
		}
		case 2:
		{
			spNormalTexture->use();

			glUniform3fv(spNormalTexture->u("viewPos"), 1, &camera->Position[0]);

			glUniform3f(spNormalTexture->u("dirLight.direction"), 0.0f, 100.0f, 0.0f);
			glUniform3f(spNormalTexture->u("dirLight.ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(spNormalTexture->u("dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
			glUniform3f(spNormalTexture->u("dirLight.specular"), 0.5f, 0.5f, 0.5f);

			glUniform3f(spNormalTexture->u("pointLights[0].position"), pointLightPositions[0][0], pointLightPositions[0][1], pointLightPositions[0][2]);
			glUniform3f(spNormalTexture->u("pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(spNormalTexture->u("pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(spNormalTexture->u("pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(spNormalTexture->u("pointLights[0].constant"), 1.0f);
			glUniform1f(spNormalTexture->u("pointLights[0].linear"), 0.09);
			glUniform1f(spNormalTexture->u("pointLights[0].quadratic"), 0.032);

			glUniform3f(spNormalTexture->u("pointLights[1].position"), pointLightPositions[1][0], pointLightPositions[1][1], pointLightPositions[1][2]);
			glUniform3f(spNormalTexture->u("pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
			glUniform3f(spNormalTexture->u("pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
			glUniform3f(spNormalTexture->u("pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
			glUniform1f(spNormalTexture->u("pointLights[1].constant"), 1.0f);
			glUniform1f(spNormalTexture->u("pointLights[1].linear"), 0.09);
			glUniform1f(spNormalTexture->u("pointLights[1].quadratic"), 0.032);

			glUniformMatrix4fv(spNormalTexture->u("P"), 1, false, glm::value_ptr(P));
			glUniformMatrix4fv(spNormalTexture->u("V"), 1, false, glm::value_ptr(V));

			M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

			glUniformMatrix4fv(spNormalTexture->u("M"), 1, false, glm::value_ptr(M));

			ourModel->Draw(*spNormalTexture);

			spSimpleTexture->use();

			glUniformMatrix4fv(spSimpleTexture->u("P"), 1, false, glm::value_ptr(P));
			glUniformMatrix4fv(spSimpleTexture->u("V"), 1, false, glm::value_ptr(V));

			M = glm::translate(M, glm::vec3(5.0f, 0.0f, 0.0f));

			glUniformMatrix4fv(spSimpleTexture->u("M"), 1, false, glm::value_ptr(M));
			glUniform3f(spSimpleTexture->u("lightPos"), 2.5f, 0.0f, 100.0f);
			glUniform3fv(spSimpleTexture->u("viewPos"), 1, &camera->Position[0]);
			ourModel->Draw(*spSimpleTexture);
			break;
		}
		case 3:
		{
			spFunnyCat->use();
			glUniformMatrix4fv(spFunnyCat->u("P"), 1, false, glm::value_ptr(P));
			glUniformMatrix4fv(spFunnyCat->u("V"), 1, false, glm::value_ptr(V));
			glUniform1f(spFunnyCat->u("amount"), sin(amount) + 1);

			M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

			glUniformMatrix4fv(spFunnyCat->u("M"), 1, false, glm::value_ptr(M));
			ourModel->Draw(*spFunnyCat);

			M = glm::translate(M, glm::vec3(0.0f, 5.0f, 0.0f));
			M = glm::rotate(M, PI / 2, glm::vec3(-1.0f, 0.0f, 0.0f));
			M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));

			glUniformMatrix4fv(spFunnyCat->u("M"), 1, false, glm::value_ptr(M));
			ourModel2->Draw(*spFunnyCat);
			break;
		}
		case 4: 
		{
			break;
		}
	}


	V = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	spSkyBox->use();
	glUniformMatrix4fv(spSkyBox->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spSkyBox->u("V"), 1, false, glm::value_ptr(V));
	glDepthMask(GL_FALSE);
	
	skybox->Draw(*spSkyBox);

	glDepthMask(GL_TRUE);
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
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		camera->ProcessKeyboard(deltaTime);
		amount += glfwGetTime();
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}
		freeOpenGLProgram(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
}
