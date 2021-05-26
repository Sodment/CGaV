#ifndef QUAD_H
#define QUAD_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "shaderprogram.h"

extern float simpleQuad[];

struct QuadTexture {
    GLuint id;
    std::string type;
};

class Quad
{
public:
    GLuint quadVAO, quadVBO;
    std::vector<QuadTexture> textures;

    Quad();
    void Draw(ShaderProgram& shader);
    void AddTexture(std::string path, std::string type);

private:
    void SetupQuad();
};
#endif
