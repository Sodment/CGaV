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
#include "shadows.h"
#include "particle_system.h"

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

// Shaders
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
ShaderProgram* spShadows;
ShaderProgram* spParticles;

// Utility
SkyBox* skybox;
PostProcessingQuad* postProcessingQuad;
ShadowsMap* shadowsMap;

//Models
PBRModel* modelFireplace;
PBRModel* modelLongTable;
PBRModel* modelRoundTable;
PBRModel* modelBench;
PBRModel* modelChair;
PBRModel* modelLamp;
PBRModel* modelCup;
PBRModel* modelCandle;
PBRModel* modelRug;
PBRModel* modelFlame;
PBRModel* modelBarrel;
PBRModel* modelBottle;
PBRModel* modelBottle2;
PBRModel* modelFireplace2;
PBRModel* modelPlate;
PBRModel* modelBowl;
PBRModel* modelSack;
PBRModel* modelApple;
PBRModel* modelCheese;
PBRModel* modelTomato;
PBRModel* modelCup_m;
PBRModel* modelShelf;
PBRModel* modelBackpack;
PBRModel* modelFirelog;
ParticleGenerator* particle;
//ParticleGenerator* particle2;
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

	//For shadows
	spShadows = new ShaderProgram("shadows.vert", "shadows.geom", "shadows.frag");

	//For particles
	spParticles = new ShaderProgram("particles.vert", NULL, "particles.frag");

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
	modelLongTable = new PBRModel("res/PBRModels/Tables/long_table.obj");
	modelRoundTable = new PBRModel("res/PBRModels/Tables/round_table.obj");
	modelBench = new PBRModel("res/PBRModels/Chairs/bench.obj");
	modelChair = new PBRModel("res/PBRModels/Chairs/chair.obj");
	modelLamp = new PBRModel("res/PBRModels/Lamp/lamp.obj");
	modelCup = new PBRModel("res/PBRModels/CandleCup/c_cup.obj");
	modelCandle = new PBRModel("res/PBRModels/Candles/candle.obj");
	modelRug = new PBRModel("res/PBRModels/Rugs/rug.obj");
	modelFlame = new PBRModel("res/PBRModels/Candles/flame.obj");
	modelBarrel = new PBRModel("res/PBRModels/Barrel/barrel.obj");
	modelBottle = new PBRModel("res/PBRModels/Vessels/bottle.obj");
	modelBottle2 = new PBRModel("res/PBRModels/Vessels/pink_love.obj");
	modelFireplace2 = new PBRModel("res/PBRModels/Fireplace/fireplace2.obj");
	modelPlate = new PBRModel("res/PBRModels/Plates/plate.obj");
	modelBowl = new PBRModel("res/PBRModels/Plates/bowl.obj");
	modelSack = new PBRModel("res/PBRModels/Sacks/sack.obj");
	modelApple = new PBRModel("res/PBRModels/Apple/apple.obj");
	modelCheese = new PBRModel("res/PBRModels/Cheese/cheese.obj");
	modelTomato = new PBRModel("res/PBRModels/Tomato/tomato.obj");
	modelCup_m = new PBRModel("res/PBRModels/Cups/cup.obj");
	modelShelf = new PBRModel("res/PBRModels/Shelves/shelf.obj");
	modelFirelog = new PBRModel("res/PBRModels/Firelog/firelog.obj");
	modelBackpack = new PBRModel("res/backpack/backpack.obj");
	particle = new ParticleGenerator(TextureFromFile("res/quads/fire2.png"), glm::vec3(4.77f, 0.5f, -7.0f),  200);
	//particle2 = new ParticleGenerator(TextureFromFile("res/quads/ja2_1.png"), glm::vec3(0.0f, 0.5f, 0.0f),  5);
	skybox = new SkyBox();
}



void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glDepthFunc(GL_LEQUAL);
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	camera = new Camera();
	postProcessingQuad = new  PostProcessingQuad(SCR_WIDTH, SCR_HEIGHT);
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

	//glUniform3fv(spPBRtexture->u("lights[4].position"), 1, &camera->Position[0]);
	glUniform3fv(spPBRtexture->u("viewPos"), 1, &camera->Position[0]);
	glUniform1f(spPBRtexture->u("lightRadius"), 10.00f);

	glUniformMatrix4fv(spPBRtexture->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spPBRtexture->u("V"), 1, false, glm::value_ptr(V));


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

	//Fireplace
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.0f, 0.5f, -7.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFireplace->Draw(*spPBRtexture);

	//Long Table
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(2.0f, 0.5f, 8.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelLongTable->Draw(*spPBRtexture);

	//Round Table
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-5.0f, 0.5f, -3.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelRoundTable->Draw(*spPBRtexture);

	//Bench
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.0f, 0.5f, 5.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBench->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.0f, 0.5f, 5.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBench->Draw(*spPBRtexture);

	//Chair
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.0f, 0.5f, -3.0f));
	M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelChair->Draw(*spPBRtexture);

	//Lamp
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.0f, 6.5f, 10.0f));
	M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelLamp->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.0f, 6.5f, 10.0f));
	M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelLamp->Draw(*spPBRtexture);

	//Candle Cup
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-2.0f, 4.65f, -9.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCup->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-5.0f, 3.8f, -3.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCup->Draw(*spPBRtexture);

	//Candle
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-5.0f, 4.0f, -3.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.0f, 6.75f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.8f, 6.4f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-0.2f, 6.4f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.0f, 6.75f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.8f, 6.4f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.2f, 6.4f, 9.4f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-2.0f, 4.85f, -9.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCandle->Draw(*spPBRtexture);

	//Rug
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 0.5f, 0.0f));
	M = glm::scale(M, glm::vec3(5.0f, 5.0f, 5.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelRug->Draw(*spPBRtexture);

	//Flame
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-5.0f, 4.55f, -3.0f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.0f, 7.3f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-1.8f, 6.95f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-0.2f, 6.95f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.0f, 7.3f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(5.8f, 6.95f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.2f, 6.95f, 9.4f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-2.0f, 5.4f, -9.0f));
	M = glm::scale(M, glm::vec3(0.08f, 0.08f, 0.08f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFlame->Draw(*spPBRtexture);

	//Barrel
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-8.0f, 0.5f, 8.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBarrel->Draw(*spPBRtexture);

	//Bottle
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-4.2f, 3.8f, -2.2f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBottle->Draw(*spPBRtexture);

	//Bottle 2
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.2f, 3.05f, 7.3f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBottle2->Draw(*spPBRtexture);

	//Fireplace2
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.0f, 6.0f, -7.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFireplace2->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.0f, 8.5f, -7.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFireplace2->Draw(*spPBRtexture);

	//Bowl
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.0f, 3.05f, 7.8f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBowl->Draw(*spPBRtexture);

	//Plate
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.5f, 3.05f, 7.3f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelPlate->Draw(*spPBRtexture);

	//Sack
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-8.0f, 3.3f, 8.0f));
	M = glm::rotate(M, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelSack->Draw(*spPBRtexture);

	//Apple
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.3f, 3.27f, 7.85f));
	M = glm::rotate(M, glm::radians(60.0f), glm::vec3(1.0f, 0.3f, 0.5f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelApple->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(3.9f, 3.25f, 7.6f));
	M = glm::rotate(M, glm::radians(-40.0f), glm::vec3(8.0f, 0.2f, 0.8f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelApple->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.0f, 3.25f, 7.95f));
	M = glm::rotate(M, glm::radians(-60.0f), glm::vec3(0.6f, 0.45f, 0.4f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelApple->Draw(*spPBRtexture);

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(4.07f, 3.47f, 7.78f));
	M = glm::rotate(M, glm::radians(10.0f), glm::vec3(0.9f, 0.3f, 0.8f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelApple->Draw(*spPBRtexture);

	//Cheese
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-6.2f, 1.5f, 8.0f));
	M = glm::rotate(M, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCheese->Draw(*spPBRtexture);

	//Tomato
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.5f, 3.05f, 7.3f));
	M = glm::scale(M, glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelTomato->Draw(*spPBRtexture);

	//Cup
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-0.4f, 3.05f, 8.0f));
	M = glm::rotate(M, glm::radians(70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelCup_m->Draw(*spPBRtexture);

	//Shelf
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-4.0f, 4.5f, -10.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelShelf->Draw(*spPBRtexture);

	//Backpack
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(-4.0f, 6.45f, -8.8f));
	M = glm::rotate(M, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	M = glm::scale(M, glm::vec3(1.0f, 1.0f, 1.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelBackpack->Draw(*spPBRtexture);

	//Firelog
	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(6.0f, 0.95f, -6.9f));
	M = glm::rotate(M, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(3.0f, 3.0f, 3.0f));

	glUniformMatrix4fv(spPBRtexture->u("M"), 1, false, glm::value_ptr(M));

	modelFirelog->Draw(*spPBRtexture);

	//Light Casters
	spPointLight->use();
	glUniformMatrix4fv(spPointLight->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spPointLight->u("V"), 1, false, glm::value_ptr(V));
	for (size_t i = 0; i < NR_LIGHTS; i++)
	{
		M = glm::mat4(1.0);
		M = glm::translate(M, pointLights[i].position);
		M = glm::scale(M, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(spPointLight->u("M"), 1, false, glm::value_ptr(M));
		glUniform3fv(spPointLight->u("color"), 1, &pointLights[i].diffuse[0]);
		cubeTest->Draw(*spPointLight);
	}

	M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(0.0f, 5.0f, 0.0f));

	//Particles
	spParticles->use();
	glUniformMatrix4fv(spParticles->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spParticles->u("V"), 1, false, glm::value_ptr(V));
	glUniform1f(spParticles->u("particleSize"), 0.2f);
	particle->Update(deltaTime);
	particle->Draw(*spParticles);
	/*
	particle2->Update(deltaTime);
	particle2->Draw(*spParticles);
	*/

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
