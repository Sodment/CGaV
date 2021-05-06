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
using namespace std;

class PBRModel
{
public:
    // model data 
    vector<PBRTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<PBRMesh> meshes;
    string directory;

    PBRModel(string const& path);

    void Draw(ShaderProgram& shader);
    void DrawMaterial(ShaderProgram& shader);

private:
    void loadModel(string const& path);

    void processNode(aiNode* node, const aiScene* scene);
    PBRMesh processMesh(aiMesh* mesh, const aiScene* scene);

    PBRMaterial loadMaterial(aiMaterial* mat);

    vector<PBRTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};
#endif
