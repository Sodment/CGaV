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
#include "mySkybox.h"

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
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	camera = new Camera();
	sp = new ShaderProgram("v_lab8.glsl", NULL, "f_lab8.glsl");
	spSkyBox = new ShaderProgram("v_skybox.glsl", NULL, "f_skybox.glsl");
	tex0 = loadTexture("res/bricks3b_diffuse.png");
	tex1 = loadTexture("res/bricks3b_normal.png");
	tex2 = loadTexture("res/bricks3b_height.png");
	tex3 = loadTexture("res/bricks3b_specular.png");
	skyBox = loadCubemap(skyboxFaces);
}

void freeOpenGLProgram(GLFWwindow* window) {
	delete camera;
	delete spSkyBox;
	delete sp;
}
//Procedura rysuj¹ca zawartoœæ sceny
void drawScene(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = camera->GetViewMatrix();

	glm::mat4 P = camera->GetPerspectiveMatrix(aspectRatio, near_clip, far_clip);

	glm::mat4 M = glm::mat4(1.0f);


	sp->use();
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices);

	glEnableVertexAttribArray(sp->a("c1"));
	glVertexAttribPointer(sp->a("c1"), 4, GL_FLOAT, false, 0, c1);

	glEnableVertexAttribArray(sp->a("c2"));
	glVertexAttribPointer(sp->a("c2"), 4, GL_FLOAT, false, 0, c2);

	glEnableVertexAttribArray(sp->a("c3"));
	glVertexAttribPointer(sp->a("c3"), 4, GL_FLOAT, false, 0, c3);

	glEnableVertexAttribArray(sp->a("texCoord0"));
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords);

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glUniform1i(sp->u("textureMap2"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glUniform1i(sp->u("textureMap3"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, tex3);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	M = glm::translate(M, glm::vec3(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));

	glDrawArrays(GL_TRIANGLES, 0, vertexCount);

	glDisableVertexAttribArray(sp->a("vertex"));
	glDisableVertexAttribArray(sp->a("c1"));
	glDisableVertexAttribArray(sp->a("c2"));
	glDisableVertexAttribArray(sp->a("c3"));
	glDisableVertexAttribArray(sp->a("texCoord0"));

	M = glm::mat4(1.0f);
	V = glm::mat4(glm::mat3(camera->GetViewMatrix()));



	spSkyBox->use();
	glDepthMask(GL_FALSE);
	glUniformMatrix4fv(spSkyBox->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spSkyBox->u("V"), 1, false, glm::value_ptr(V));

	glEnableVertexAttribArray(spSkyBox->a("aPos"));
	glVertexAttribPointer(spSkyBox->a("aPos"), 3, GL_FLOAT, false, 0, skyboxVertices);

	glUniform1i(spSkyBox->u("skybox"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);

	glDrawArrays(GL_TRIANGLES, 0, 36);

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

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);

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
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}
		freeOpenGLProgram(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
}
