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
#include"shaderprogram.h"

struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    float Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};

class ParticleGenerator
{
public:
    ParticleGenerator(GLuint texture, unsigned int amount);
    void Draw(ShaderProgram& shader);
    void Update(float dt, glm::vec3 pos, unsigned int newParticles, glm::vec2 offset);
private:
    std::vector<Particle> particles;
    unsigned int amount;
    GLuint VAO, texture;
    void Setup();
    unsigned int FirstUnusedParticle();
    void RespawnParticle(Particle& particle, glm::vec3 pos, glm::vec2 offset);
};

#endif

