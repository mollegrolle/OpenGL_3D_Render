#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>

#include <glm/glm.hpp>

#include "OBJLoader.h"

bool loadOBJ(
	const char * path,
	std::vector < glm::vec3 > & out_vertices,
	std::vector < glm::vec2 > & out_uvs,
	std::vector < glm::vec3 > & out_normals,
	std::vector < glm::vec3 > & out_tangents,
	std::vector < glm::vec3 > & out_bitangents
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec2 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	FILE * file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file !\n");
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

				   // else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}
	}

	glm::vec3 vertex;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i] - 1;
		unsigned int uvIndex = uvIndices[i] - 1;
		unsigned int normalIndex = normalIndices[i] - 1;

		// Get the attributes thanks to the index
		vertex = temp_vertices[vertexIndex];
		uv = temp_uvs[uvIndex];
		normal = temp_normals[normalIndex];

		// Calculate tangent
		if (i % 3 == 0)
		{
			glm::vec3 deltaPos1 = temp_vertices[vertexIndices[i + 1] - 1] - temp_vertices[vertexIndices[i + 0] - 1];
			glm::vec3 deltaPos2 = temp_vertices[vertexIndices[i + 2] - 1] - temp_vertices[vertexIndices[i + 0] - 1];
			glm::vec2 deltaUV1 = temp_uvs[vertexIndices[i + 1] - 1] - temp_uvs[vertexIndices[i + 0] - 1];
			glm::vec2 deltaUV2 = temp_uvs[vertexIndices[i + 2] - 1] - temp_uvs[vertexIndices[i + 0] - 1];
		
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

			tangent = f * (deltaPos1 * deltaPos2.y - deltaPos2 * deltaPos1.y);
			bitangent = f * (deltaPos2 * deltaPos1.x - deltaPos1 * deltaPos2.x);

			//tangent = glm::cross(bitangent, normal);

			tangent = glm::normalize(tangent);		
			bitangent = glm::normalize(bitangent);
		}

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
		out_tangents.push_back(tangent);
		out_bitangents.push_back(bitangent);

	}
	fclose(file);
	return true;
}
