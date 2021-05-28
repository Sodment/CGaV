#ifndef PARTICLE_SYSTEM_H
#define PARTICLE SYSTEM_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <random>
#include <time.h>
#include"shaderprogram.h"


struct Particle {
    glm::vec3 Position;
    float Life;
};

class ParticleGenerator
{
public:
    ParticleGenerator(GLuint texture, glm::vec3 pos,unsigned int amount);
    void Draw(ShaderProgram& shader);
    void Update(float dt);
private:
    glm::vec3 position;
    Particle RespawnParticle();
    std::vector<Particle> particles;
    unsigned int amount;
    float positions[400];
    GLuint VAO,positionsBuffer, texture;
};

#endif

