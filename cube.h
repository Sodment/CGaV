#ifndef CUBE_H
#define CUBE_H

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

extern float simpleCube[];

class Cube
{
public:
    GLuint cubeVAO, cubeVBO;
    Cube();
    void Draw(ShaderProgram& shader);

private:
    void SetupCube();
};
#endif
