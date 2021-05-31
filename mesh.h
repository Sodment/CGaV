#ifndef MESH_H
#define MESH_H

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
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

struct Material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    float Shininess;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    Material material;
    GLuint VAO;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Material mat);
    void Draw(const ShaderProgram& shader);
    void DrawMaterial(const ShaderProgram& shader);

private:
    GLuint VBO, EBO;
    void setupMesh();
};
#endif