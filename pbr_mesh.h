#ifndef PBR_MESH_H
#define PBR_MESH_H

#include <Gl/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "shaderprogram.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

struct Material {
    glm::vec3 Albedo;
    float Metallic;
    float Roughness;
    float Ao;
};

class PBRMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    Material material;
    GLuint VAO;
    PBRMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material mat);
    void Draw(ShaderProgram& shader);
    void DrawMaterial(ShaderProgram& shader);

private:
    GLuint VBO, EBO;
    void setupPBRMesh();
};
#endif
