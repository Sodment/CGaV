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

float quadVertices[] = 
{
       -1.0f,  1.0f,  0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

       -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

void SetPostPRoccesingKernel(ShaderProgram& sp, float k[9])
{
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[0]"), k[0]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[1]"), k[1]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[2]"), k[2]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[3]"), k[3]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[4]"), k[4]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[5]"), k[5]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[6]"), k[6]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[7]"), k[7]);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, "kernel[8]"), k[8]);
}

class PostQuad
{
public:
	GLuint quadVAO, quadVBO;
	GLuint framebuffer,tcb, rbo;

    PostQuad(unsigned int width, unsigned int height) 
    { 
        SetupQuad();
        SetupFBO();
        SetupTCB(width, height);
        SetupRBO(width, height);
    };

    void UpdateResolution(int width, int height)
    {
        SetupTCB(width, height);
        SetupRBO(width, height);
    }

    void SetupFBO()
    {
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }

    void SetupTCB(const int SCR_WIDTH, const int SCR_HEIGHT)
    {
        glGenTextures(1, &tcb);
        glBindTexture(GL_TEXTURE_2D, tcb);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tcb, 0);
    }

	void SetupRBO(const int SCR_WIDTH, const int SCR_HEIGHT)
	{
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}

    void SetupQuad()
    {
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void Draw(ShaderProgram& shader)
    {
        glUniform1i(glGetUniformLocation(shader.shaderProgram, "screentexture"), 0);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, tcb);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};
#endif