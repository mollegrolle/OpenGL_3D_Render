#pragma once

#include <iostream>
#include <vector>
#include <GLM\glm.hpp>
#include "Shader.h"
#include "Transform.h"

class BoundingBox
{
public:
	// Mesh data
	std::vector<glm::vec3> position;
	unsigned int VAO;

	// Functions
	BoundingBox();	
	void loadVertices(const std::vector<glm::vec3> meshVertices);
	void Draw(Shader shader);
private:
	Transform transform;

	GLushort elements;
	GLuint vbo_vertices;
	GLuint ibo_elements;
};

