#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "shaderprogram.h"

class GBufferSpecular
{
public:
	GLuint gBuffer;
	GLuint gPosition, gNormal, gAlbedoSpec;
	GLuint rboDepth;
	GBufferSpecular(unsigned int width, unsigned int height);
private:
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	void ConfigureGBuffer();
	void SetupPositionColorBuffer(const int SCR_WIDTH, const int SCR_HEIGHT);
	void SetupNormalColorBuffer(const int SCR_WIDTH, const int SCR_HEIGHT);
	void SetupColorSpecularBuffer(const int SCR_WIDTH, const int SCR_HEIGHT);
	void SetupDepthBuffer(const int SCR_WIDTH, const int SCR_HEIGHT);
};