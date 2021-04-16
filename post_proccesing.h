#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "shaderprogram.h"

float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
       // positions   // texCoords
       -1.0f,  1.0f,  0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

       -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
};

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
        printf("DUPA1\n");
    };

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