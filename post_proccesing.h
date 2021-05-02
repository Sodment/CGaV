#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "shaderprogram.h"

extern float quadVertices[];

void SetPostProccesingKernel(ShaderProgram& sp, float k[9]);

class PostProcessingQuad
{
public:
	GLuint quadVAO, quadVBO;
	GLuint framebuffer,tcb, rbo;

    PostProcessingQuad(unsigned int width, unsigned int height);
    void Draw(ShaderProgram& shader);
    void UpdateResolution(int width, int height);

private:
    void SetupFBO();
    void SetupTCB(const int SCR_WIDTH, const int SCR_HEIGHT);
    void SetupRBO(const int SCR_WIDTH, const int SCR_HEIGHT);
    void SetupQuad();
};
#endif