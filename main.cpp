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
#include "model.h"
#include "pbr_model.h"
#include "skybox.h"
#include "post_proccesing.h"
#include "lights.h"
#include "image.h"
#include "g_buffer_specular.h"
#include "quad.h"
#include "cube.h"

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 1024;

float aspectRatio = 1280/1024;
float nearClip = 0.1f;
float farClip = 1000.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float timeSinceStart = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool drunk = false;

Camera* camera;
ShaderProgram* spSkyBox;
ShaderProgram* spDiffuseOnly;
ShaderProgram* spSimpleTexture;
ShaderProgram* spNormalTexture;
ShaderProgram* spFunnyCat;
ShaderProgram* spMaterial;
ShaderProgram* spSimpleMaterial;
ShaderProgram* spScreenShader;
ShaderProgram* spPBRmaterial;
ShaderProgram* spPBRtexture;
ShaderProgram* spDeferredSpecularGeomPass;
ShaderProgram* spDeferredSpecularLightPass;
ShaderProgram* spPointLight;
ShaderProgram* spWater;
SkyBox* skybox;
PBRModel* modelFireplace;
PostProcessingQuad* postProcessingQuad;
GBufferSpecular* gBufferSpecular;
Quad* quadFloor;
Quad* quadWalls;
Cube* cubeTest;

float offset = 1.0f / 300.0f;
float offsets[9][2] = {
	{ -offset,  offset  },  // top-left
	{  0.0f,    offset  },  // top-center
	{  offset,  offset  },  // top-right
	{ -offset,  0.0f    },  // center-left
	{  0.0f,    0.0f    },  // center-center
	{  offset,  0.0f    },  // center - right
	{ -offset, -offset  },  // bottom-left
	{  0.0f,   -offset  },  // bottom-center
	{  offset, -offset  }   // bottom-right    
};

float blur_kernel[9] = {
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
		2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
		1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
};

float kernel[9] = {
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
		if (key == GLFW_KEY_W) { direction.Forward = true; }
		if (key == GLFW_KEY_S) { direction.Backward = true; }
		if (key == GLFW_KEY_A) { direction.Left = true; }
		if (key == GLFW_KEY_D) { direction.Right = true; }
		if (key == GLFW_KEY_SPACE) { direction.Up = true; }
		if (key == GLFW_KEY_LEFT_SHIFT) { direction.Down = true; }
		if (key == GLFW_KEY_F) { drunk = true; }

	}

	if (action == GLFW_RELEASE) 
	{
		if (key == GLFW_KEY_W) { direction.Forward = false; }
		if (key == GLFW_KEY_S) { direction.Backward = false; }
		if (key == GLFW_KEY_A) { direction.Left = false; }
		if (key == GLFW_KEY_D) { direction.Right = false; }
		if (key == GLFW_KEY_SPACE) { direction.Up = false; }
		if (key == GLFW_KEY_LEFT_SHIFT) { direction.Down = false; }
		if (key == GLFW_KEY_F) { drunk = false; }
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
	printf("Offset: %lf \n", yoffset);
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

	//Deferred shading
	spDeferredSpecularGeomPass = new ShaderProgram("df_diff_spec_geom_pass.vert", NULL, "df_diff_spec_geom_pass.frag");
	spDeferredSpecularLightPass = new ShaderProgram("df_diff_spec_light_pass.vert", NULL, "df_diff_spec_light_pass.frag");

	//With multiple lights
	spDiffuseOnly = new ShaderProgram("diffuse_only.vert", NULL, "diffuse_only.frag");
	spSimpleTexture = new ShaderProgram("diffuse_specular.vert", NULL, "diffuse_specular.frag");
	spNormalTexture = new ShaderProgram("diff_spec_norm.vert", NULL, "diff_spec_norm.frag");
	spPBRtexture = new ShaderProgram("pbr_texture.vert", NULL, "pbr_texture.frag");

	//For materials
	spMaterial = new ShaderProgram("material.vert", NULL, "material.frag");
	spSimpleMaterial = new ShaderProgram("simple_material.vert", "simple_material.geom", "simple_material.frag"); //is only for seeing the model drawn with diffuse
	spPBRmaterial = new ShaderProgram("pbr_material.vert", NULL, "pbr_material.frag");

	//For light casters
	spPointLight = new ShaderProgram("light_cube.vert", NULL, "light_cube.frag");

	//For water
	spWater = new ShaderProgram("water.vert", NULL, "water.frag");

	//Thrash
	spFunnyCat = new ShaderProgram("funnyCat.vert", "funnyCat.geom", "funnyCat.frag");
}

void initModels()
{
	cubeTest = new Cube();
	quadFloor = new Quad();
	quadFloor->AddTexture("res/quads/redbricks2b-albedo.png", "texture_diffuse");
	quadFloor->AddTexture("res/quads/redbricks2b-metalness.png", "texture_metallic");
	quadFloor->AddTexture("res/quads/redbricks2b-rough.png", "texture_roughness");
	quadFloor->AddTexture("res/quads/redbricks2b-normal.png", "texture_normal");
	quadFloor->AddTexture("res/quads/redbricks2b-ao.png", "texture_ao");

	quadWalls = new Quad();
	quadWalls->AddTexture("res/quads/wornpaintedwoodsiding-albedo.png", "texture_diffuse");
	quadWalls->AddTexture("res/quads/wornpaintedwoodsiding-metalness.png", "texture_metallic");
	quadWalls->AddTexture("res/quads/wornpaintedwoodsiding-roughness.png", "texture_roughness");
	quadWalls->AddTexture("res/quads/wornpaintedwoodsiding-normal-ogl.png", "texture_normal");
	quadWalls->AddTexture("res/quads/wornpaintedwoodsiding-ao.png", "texture_ao");

	modelFireplace = new PBRModel("res/PBRModels/Fireplace/fireplace.obj");
}



void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	camera = new Camera();
	skybox = new SkyBox();
	postProcessingQuad = new  PostProcessingQuad(SCR_WIDTH, SCR_HEIGHT);
	gBufferSpecular = new GBufferSpecular(SCR_WIDTH, SCR_HEIGHT);
}

void freeOpenGLProgram(GLFWwindow* window) {
	delete camera, postProcessingQuad,
	delete spSkyBox,spScreenShader,spFunnyCat,spFunnyCat;
}

void drawScene(GLFWwindow* window) {
	//Binding post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessingQuad->framebuffer);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 V = camera->GetViewMatrix();

	glm::mat4 P = camera->GetPerspectiveMatrix(aspectRatio, nearClip, farClip);

	glm::mat4 M = glm::mat4(1.0f);

	spPBRtexture->use();
	SetMulPBRLight(*spPBRtexture, pointLights, NR_LIGHTS);

	//glUniform3fv(spPBRtexture->u("lights[0].position"), 1, &camera->Position[0]);
	glUniform1f(spPBRtexture->u("lightRadius"), 100.0f);

	glUniformMatrix4fv(spPBRtexture->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spPBRtexture->u("V"), 1, false, glm::value_ptr(V));

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.0f, 0.5f, -7.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFireplace->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadFloor->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 9.5f, 0.0f));
	M = glm::rotate(M,glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadWalls->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 0.0f, -9.5f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadWalls->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 9.5f));
	M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadWalls->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(9.5f, 0.0f, 0.0f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadWalls->Draw(*spPBRtexture);


	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-9.5f, 0.0f, 0.0f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	quadWalls->Draw(*spPBRtexture);

	spPointLight->use();
	glUniformMatrix4fv(spPointLight->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spPointLight->u("V"), 1, false, glm::value_ptr(V));
	for (size_t i = 0; i < NR_LIGHTS; i++)
	{
		M = glm::mat4(1.0);
		M = glm::translate(M, pointLights[i].position);
		M = glm::scale(M, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(spPointLight->u("M"), 1, false, glm::value_ptr(M));
		glUniform3fv(spPointLight->u("color"), 1, &pointLights[i].diffuse[0]);
		cubeTest->Draw(*spPointLight);
	}

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
	glUniform1f(spScreenShader->u("time"), timeSinceStart);
	glUniform2fv(spScreenShader->u("offsets"), 9, (float*)offsets);
	if (drunk)
	{
		glUniform1i(spScreenShader->u("drunk"), GL_TRUE);
		glUniform1fv(spScreenShader->u("blur_kernel"), 9, blur_kernel);
		float sp = glm::clamp(sin(timeSinceStart) + 1.0f, 0.8f, 1.3f);
		glUniform1f(spScreenShader->u("speed"), sp);
		camera->ProcessMouseScroll(0.06 * sin(timeSinceStart));
	}
	else
	{
		glUniform1i(spScreenShader->u("drunk"), GL_FALSE);
		glUniform1fv(spScreenShader->u("blur_kernel"), 9, kernel);
	}
	postProcessingQuad->Draw(*spScreenShader);

	glfwSwapBuffers(window);
	//printf("Time sice start: %f\n", timeSinceStart);
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
		fprintf(stderr, "Error when creating the window.\n");
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
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		camera->ProcessKeyboard(deltaTime);
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
		timeSinceStart += deltaTime;
	}
		freeOpenGLProgram(window);
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_SUCCESS);
}
