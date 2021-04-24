#pragma once
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

void SetPointLight(ShaderProgram* sp, PointLight light, int index)
{

}

void SetDirLight(ShaderProgram* sp, DirLight light)
{
    glUniform3fv(sp->u("dirLight.direction"), 1, light.direction);
    glUniform3f(sp->u("dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(sp->u("dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(sp->u("dirLight.specular"), 0.5f, 0.5f, 0.5f);
}
