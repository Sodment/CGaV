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

DirLight dirLight =
{
    glm::vec3(0.0f, 100.0f, 0.0f),
    glm::vec3(0.05f, 0.05f, 0.05f),
    glm::vec3(0.4f, 0.4f, 0.4f),
    glm::vec3(0.5f, 0.5f, 0.5f),
};

void SetPointLight(ShaderProgram& sp, PointLight light, int index)
{
    char* string;
    sprintf(string, "pointLights[%d].position", index);
    //glUniform3f(spNormalTexture->u("pointLights[0].position"), pointLightPositions[0][0], pointLightPositions[0][1], pointLightPositions[0][2]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.position[0]);
   // glUniform3f(spNormalTexture->u("pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    //glUniform3f(spNormalTexture->u("pointLights[0].diffuse"), 0.0f, 0.8f, 0.8f);
   // glUniform3f(spNormalTexture->u("pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
   //glUniform1f(spNormalTexture->u("pointLights[0].constant"), 1.0f);
   // glUniform1f(spNormalTexture->u("pointLights[0].linear"), 0.09);
    //glUniform1f(spNormalTexture->u("pointLights[0].quadratic"), 0.032);
}

void SetDirLight(ShaderProgram& sp, DirLight light)
{
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.direction"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.ambient"), 1, &light.ambient[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.diffuse"), 1, &light.diffuse[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.specular"), 1, &light.specular[0]);
}
