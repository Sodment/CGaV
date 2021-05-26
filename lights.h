#ifndef LIGHTS_H
#define LIGHTS_H
#define NR_LIGHTS 5

#pragma warning(disable: 4996)
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "shaderprogram.h"

typedef struct pointlight 
{
    glm::vec3 position;

    float constant;
    float linear;
    float quadratic;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
}PointLight;

typedef struct dirlight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
}DirLight;

extern DirLight dirLight;
extern PointLight pointLights[NR_LIGHTS];

void SetPointLight(ShaderProgram& sp, PointLight light, int index);
void SetMulPointLight(ShaderProgram& sp, PointLight lights[], int size);
void SetDirLight(ShaderProgram& sp, DirLight light);
void SetPBRLight(ShaderProgram& sp, PointLight light, int index);
void SetMulPBRLight(ShaderProgram& sp, PointLight lights[], int size);
#endif
