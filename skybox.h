#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "shaderprogram.h"

extern const char* faces[6];
extern float skyboxVertices[];

class SkyBox 
{
public:
    GLuint VAO;

    SkyBox();
    void Draw(ShaderProgram& shader);

private:
    GLuint VBO, skyboxTexture;

    GLuint loadCubemap(const char* faces[6]);
    void setupSkybox();
};
#endif
