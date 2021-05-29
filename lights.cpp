#pragma warning(disable: 4996)
#include "lights.h"

DirLight dirLight =
{
    glm::vec3(-0.2f, -1.0f, -0.3f),
    glm::vec3(0.05f, 0.05f, 0.05f),
    glm::vec3(0.4f, 0.4f, 0.4f),
    glm::vec3(0.5f, 0.5f, 0.5f),
};

PointLight pointLights[NR_LIGHTS] =
{
//1
glm::vec3(-1.0f, 7.3f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//2
glm::vec3(-1.8f, 6.95f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//3
glm::vec3(-0.2f, 6.95f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//4
glm::vec3(5.0f, 7.3f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//5
glm::vec3(5.8f, 6.95f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//6
glm::vec3(4.2f, 6.95f, 9.4f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),

//7
glm::vec3(-5.0f, 4.55f, -3.0f),
1.0f,
0.14f,
0.007f,
glm::vec3(0.05f, 0.05f, 0.05f),
glm::vec3(1.0f, 0.86f, 0.6f),
glm::vec3(1.0f, 1.0f, 1.0f),
};

void SetPointLight(ShaderProgram& sp, PointLight light, int index)
{
    char string[26];
    sprintf(string, "pointLights[%d].position", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.position[0]);
    sprintf(string, "pointLights[%d].ambient", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.ambient[0]);
    sprintf(string, "pointLights[%d].diffuse", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.diffuse[0]);
    sprintf(string, "pointLights[%d].specular", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.specular[0]);
    sprintf(string, "pointLights[%d].constant", index);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, string), light.constant);
    sprintf(string, "pointLights[%d].linear", index);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, string), light.linear);
    sprintf(string, "pointLights[%d].quadratic", index);
    glUniform1f(glGetUniformLocation(sp.shaderProgram, string), light.quadratic);
}

void SetMulPointLight(ShaderProgram& sp, PointLight lights[], int size)
{
    for (int i = 0; i < size; i++)
    {
        char string[26];
        sprintf(string, "pointLights[%d].position", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].position[0]);
        sprintf(string, "pointLights[%d].ambient", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].ambient[0]);
        sprintf(string, "pointLights[%d].diffuse", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].diffuse[0]);
        sprintf(string, "pointLights[%d].specular", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].specular[0]);
        sprintf(string, "pointLights[%d].constant", i);
        glUniform1f(glGetUniformLocation(sp.shaderProgram, string), lights[i].constant);
        sprintf(string, "pointLights[%d].linear", i);
        glUniform1f(glGetUniformLocation(sp.shaderProgram, string), lights[i].linear);
        sprintf(string, "pointLights[%d].quadratic", i);
        glUniform1f(glGetUniformLocation(sp.shaderProgram, string), lights[i].quadratic);
    }
}

void SetDirLight(ShaderProgram& sp, DirLight light)
{

    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.direction"), 1, &light.direction[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.ambient"), 1, &light.ambient[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.diffuse"), 1, &light.diffuse[0]);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, "dirLight.specular"), 1, &light.specular[0]);
}

void SetPBRPLight(ShaderProgram& sp, PointLight light, int index)
{
    char string[26];
    sprintf(string, "lights[%d].position", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.position[0]);
    sprintf(string, "lights[%d].color", index);
    glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &light.diffuse[0]);
    sprintf(string, "lights[%d].constant", index);
}

void SetMulPBRLight(ShaderProgram& sp, PointLight lights[], int size)
{
    char string[26];
    for (int i = 0; i < size; i++)
    {
        sprintf(string, "lights[%d].position", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].position[0]);
        sprintf(string, "lights[%d].color", i);
        glUniform3fv(glGetUniformLocation(sp.shaderProgram, string), 1, &lights[i].diffuse[0]);
    }
}