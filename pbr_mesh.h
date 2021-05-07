#ifndef PBR_MESH_H
#define PBR_MESH_H

#include <Gl/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <vector>
#include "shaderprogram.h"

struct PBRVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct PBRTexture {
    GLuint id;
    std::string type;
    std::string path;
};

struct PBRMaterial {
    glm::vec3 Albedo;
    float Metallic;
    float Roughness;
    float Ao;
};

class PBRMesh {
public:
    std::vector<PBRVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<PBRTexture> textures;
    PBRMaterial material;
    GLuint VAO;
    PBRMesh(std::vector<PBRVertex> vertices, std::vector<unsigned int> indices, std::vector<PBRTexture> textures, PBRMaterial mat);
    void Draw(ShaderProgram& shader);
    void DrawMaterial(ShaderProgram& shader);

private:
    GLuint VBO, EBO;
    void setupPBRMesh();
};
#endif
