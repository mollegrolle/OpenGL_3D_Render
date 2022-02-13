#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLM\glm.hpp>

class Shader
{
public:

	GLuint ProgramID;

	Shader();
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL, const char* tessControlPath = NULL, const char* tessEvaluationPath = NULL);

	void loadShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL, const char* tessControlPath = NULL, const char* tessEvaluationPath = NULL);

	void useProgram();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec3(const std::string &name, glm::vec3 value) const;
	void setMat4(const std::string &name, glm::mat4 value) const;
	
private:
};