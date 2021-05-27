#ifndef SHADOWS_H
#define SHADOWS_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

class ShadowsMap 
{
public:
	GLuint depthMapFBO;
	GLuint depthCubemap;
	std::vector<glm::mat4> shadowTransforms;
	ShadowsMap(unsigned int width, unsigned int height);
	void SetShadowMat(const int SCR_WIDTH, const int SCR_HEIGHT, glm::vec3 lightPos, float near_plane, float far_plane);
private:
	void SetupDepthFBO();
	void SetubDepthCubemap(const int SCR_WIDTH, const int SCR_HEIGHT);

};


#endif

