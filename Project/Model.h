#pragma once

#include "Common.h"
#include <SOIL2\SOIL2.h>
#include <SOIL2\stb_image.h>

#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"
#include "OBJLoader.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>


unsigned int CustomTextureColor(glm::vec4 color);
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

class Model
{
public:
	Model();
	Model(std::string path);
	Model(std::vector<glm::vec3> vertices, std::vector<glm::vec2> texCoords, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents, std::vector<glm::vec3> bitangents);
	Model(std::string path, char a); //Obj parser

	void loadModelAssimp(std::string path);
	void loadModelObjParse(const char *path);

	void loadMaterial(std::string path, const std::string type);
	
	void setColor(glm::vec3 color);
	void setDiffuse(glm::vec3 diffuse);
	void setSpecular(glm::vec3 specular);
	void setNormal(glm::vec3 normal);
	void setHeight(glm::vec3 height);

	void Draw(Shader shader);

	void setTranslate(glm::vec3 translate);
	void setRotate(glm::vec3 rotate);
	void setScale(glm::vec3 scale);

	void setDrawMode(GLenum drawMode);

	glm::vec3 getTranslate();
	glm::vec3 getRotate();
	glm::vec3 getScale();
	glm::mat4 getModel();

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
	std::string directory;

	Shader shader;
	Transform transform;

	aiScene *generateScene(std::vector<glm::vec3> vertices1, std::vector<glm::vec2> texCoords1, std::vector<glm::vec3> normals1);

	void processScene(const aiScene * scene);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	std::vector<Vertex> processVertices(aiMesh * mesh);
	std::vector<unsigned int> processIndices(aiMesh * mesh);
	std::vector<Texture> processTextures(aiMesh * mesh, const aiScene * scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	
};