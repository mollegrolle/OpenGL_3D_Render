 #pragma once

#include <iostream>
#include <vector>
#include <GLM\glm.hpp>
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	// Mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	glm::vec3 color;
	unsigned int VAO;


	// Functions
	Mesh(const std::vector<Vertex> vertices, const std::vector<unsigned int> indices, const std::vector<Texture> textures);
	void setDrawMode(GLenum drawMode);
	void Draw(Shader shader);
private:
	unsigned int VBO, EBO;

	GLenum drawMode;

	// Functions
	// initializes all the buffer objects/arrays
	void setupMesh();
};

