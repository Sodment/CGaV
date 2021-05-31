#ifndef PBR_MODEL_H
#define PBR_MODEL_H

#include <GL/glew.h> 
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shaderprogram.h"
#include "pbr_mesh.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class PBRModel
{
public:
    // model data 
    std::vector<PBRTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<PBRMesh> meshes;
    std::string directory;

    PBRModel(std::string const& path);

    void Draw(const ShaderProgram& shader);
    void DrawMaterial(const ShaderProgram& shader);

private:
    void loadModel(std::string const& path);

    void processNode(aiNode* node, const aiScene* scene);
    PBRMesh processMesh(aiMesh* mesh, const aiScene* scene);

    PBRMaterial loadMaterial(aiMaterial* mat);

    std::vector<PBRTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
#endif
