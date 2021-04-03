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
float far_clip = 100.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

Camera* camera;
ShaderProgram* sp;
ShaderProgram* spSkyBox;
ShaderProgram* spBackpack;
ShaderProgram* spFunnyCat;
Model* ourModel;
Model* ourModel2;
SkyBox* skybox;


float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
float* c1 = myCubeC1;
float* c2 = myCubeC2;
float* c3 = myCubeC3;
int vertexCount = myCubeVertexCount;

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	//stbi_set_flip_vertically_on_load(true);
	camera = new Camera();
	sp = new ShaderProgram("v_lab8.glsl", NULL, "f_lab8.glsl");
	spSkyBox = new ShaderProgram("v_skybox.glsl", NULL, "f_skybox.glsl");
	spBackpack = new ShaderProgram("v_backpack.glsl", NULL, "f_backpack.glsl");
	spFunnyCat = new ShaderProgram("v_funnyCat.glsl", "g_funnyCat.glsl", "f_funnyCat.glsl");
	//tex0 = loadTexture("res/bricks/bricks3b_diffuse.png");
	//tex1 = loadTexture("res/bricks/bricks3b_normal.png");
	//tex2 = loadTexture("res/bricks/bricks3b_height.png");
	//tex3 = loadTexture("res/bricks/bricks3b_specular.png");
	ourModel = new Model("res/backpack/backpack.obj");
	ourModel2 = new Model("res/cat/cat.obj");
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

	/*spBackpack->use();

	glUniformMatrix4fv(spBackpack->u("projection"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spBackpack->u("view"), 1, false, glm::value_ptr(V));

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spBackpack->u("model"), 1, false, glm::value_ptr(M));
	ourModel->Draw(*spBackpack);*/

	spFunnyCat->use();
	M = glm::translate(M, glm::vec3(0.0f, 5.0f, 0.0f));
	M = glm::rotate(M, PI / 2, glm::vec3(-1.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(0.1f, 0.1f, 0.1f));
	glUniformMatrix4fv(spFunnyCat->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spFunnyCat->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(spFunnyCat->u("M"), 1, false, glm::value_ptr(M));
	glUniform1f(spFunnyCat->u("amount"), sin(amount) + 1);
	ourModel2->Draw(*spFunnyCat);

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
