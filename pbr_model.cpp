#include "pbr_model.h"
#include "image.h"


PBRModel::PBRModel(std::string const& path)
{
	loadModel(path);
}

// draws the model, and thus all its meshes
void PBRModel::Draw(const ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void PBRModel::DrawMaterial(const ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].DrawMaterial(shader);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void PBRModel::loadModel(std::string const& path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void PBRModel::processNode(aiNode* node, const aiScene* scene)
{
	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

PBRMesh PBRModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	std::vector<PBRVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<PBRTexture> textures;
	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces);
	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		PBRVertex vertex;
		glm::vec3 vector; //placeholder vector since assimp uses its own vector class
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// now walk through each of the mesh's faces
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	PBRMaterial mat = loadMaterial(material);

	// 1. albedo maps
	std::vector<PBRTexture> albedoeMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), albedoeMaps.begin(), albedoeMaps.end());
	// 2. ao maps
	std::vector<PBRTexture> aoMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_ao");
	textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
	// 3. metallic maps
	std::vector<PBRTexture> metallicMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_metallic");
	textures.insert(textures.end(), metallicMaps.begin(), metallicMaps.end());
	// 4. normal maps
	std::vector<PBRTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 5. roughness maps
	std::vector<PBRTexture> roughnessMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_roughness");
	textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());

	// return a mesh object created from the extracted mesh data
	return PBRMesh(vertices, indices, textures, mat);
}



PBRMaterial PBRModel::loadMaterial(aiMaterial* mat) {
	PBRMaterial material;
	aiColor3D color(0.f, 0.f, 0.f);
	float shininess;

	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	material.Albedo = glm::vec3(color.r, color.g, color.b);

	mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	material.Metallic = color.r;

	mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
	material.Roughness = color.r;

	mat->Get(AI_MATKEY_SHININESS, shininess);
	material.Ao = shininess;

	return material;
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<PBRTexture> PBRModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<PBRTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			PBRTexture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}