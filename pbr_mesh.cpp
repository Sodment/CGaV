#include "pbr_mesh.h"

PBRMesh::PBRMesh(std::vector<PBRVertex> vertices, std::vector<unsigned int> indices, std::vector<PBRTexture> textures, PBRMaterial mat)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->material = mat;
    // now that we have all the required data, set the vertex buffers and its attribute pointers.
    setupPBRMesh();
}

// render the mesh
void PBRMesh::Draw(const ShaderProgram& shader)
{
    // bind appropriate textures
    unsigned int diffuseNr = 1;
    unsigned int metalnessrNr = 1;
    unsigned int normalNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int aoNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_roughness")
        {
            number = std::to_string(roughnessNr++);
        }
        else if (name == "texture_metallic")
        {
            number = std::to_string(metalnessrNr++);
        }
        else if (name == "texture_ao")
        {
            number = std::to_string(aoNr++);
        }
        else if (name == "texture_normal")
        {
            number = std::to_string(normalNr++);
        }

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.shaderProgram, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void PBRMesh::DrawMaterial(const ShaderProgram& shader)
{

    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "material.albedo"), 1, &this->material.Albedo[0]);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.roughness"), this->material.Roughness);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.metallic"), this->material.Metallic);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.ao"), this->material.Ao);
    /*glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    glUniform3fv(glGetUniformLocation(shader.shaderProgram, "material.albedo"), 1, &color[0]);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.roughness"), 0.3f);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.metallic"), 0.0f);
    glUniform1f(glGetUniformLocation(shader.shaderProgram, "material.ao"), 1.0f);*/

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //Clean for the sake of cleaning
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void PBRMesh::setupPBRMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PBRVertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PBRVertex), (void*)offsetof(PBRVertex, TexCoords));

    glBindVertexArray(0);
}