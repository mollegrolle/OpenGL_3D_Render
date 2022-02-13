#include "Shader.h"

Shader::Shader()
{
}

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath, const char* tessControlPath, const char* tessEvaluationPath)
{
	loadShader(vertexPath, fragmentPath, geometryPath, tessControlPath, tessEvaluationPath);
}

void Shader::loadShader(const char * vertexPath, const char * fragmentPath, const char * geometryPath, const char * tessControlPath, const char * tessEvaluationPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::string tessControlCode;
	std::string tessEvaluationCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	std::ifstream tcShaderFile;
	std::ifstream teShaderFile;

	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// if geometry shader path is present, also load a geometry shader
		if (geometryPath != NULL || geometryPath == "")
		{
			gShaderFile.open(geometryPath);
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}
		// if tessellation control shader path is present, also load a tessellation control shader
		if (tessControlPath != NULL || tessControlPath == "")
		{
			tcShaderFile.open(tessControlPath);
			std::stringstream tcShaderStream;
			tcShaderStream << tcShaderFile.rdbuf();
			tcShaderFile.close();
			tessControlCode = tcShaderStream.str();
		}
		// if tessellation evaluation shader path is present, also load a tessellation evaluation shader
		if (tessEvaluationPath != NULL || tessEvaluationPath == "")
		{
			teShaderFile.open(tessEvaluationPath);
			std::stringstream teShaderStream;
			teShaderStream << teShaderFile.rdbuf();
			teShaderFile.close();
			tessEvaluationCode = teShaderStream.str();
		}
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	GLuint vertex, fragment;

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// Check Vertex Shader
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(vertex, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Check Fragment Shader
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(fragment, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// geometry shader
	// if gs is given, compile geometry shader
	GLuint geometry;
	if (geometryPath != NULL || geometryPath == "")
	{
		const char * gShaderCode = geometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

		// Check Geomatry Shader
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(geometry, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> GeometryShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(geometry, InfoLogLength, NULL, &GeometryShaderErrorMessage[0]);
			printf("%s\n", &GeometryShaderErrorMessage[0]);
		}
	}

	// tessellation control shader
	GLuint tessControl;
	if (tessControlPath != NULL || tessControlPath == "")
	{

		glPatchParameteri(GL_PATCH_VERTICES, 3);

		const char * tcShaderCode = tessControlCode.c_str();
		tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(tessControl, 1, &tcShaderCode, NULL);
		glCompileShader(tessControl);

		// Check Tessellation Control Shader
		glGetShaderiv(tessControl, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(tessControl, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> TessControlShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(tessControl, InfoLogLength, NULL, &TessControlShaderErrorMessage[0]);
			printf("%s\n", &TessControlShaderErrorMessage[0]);
		}

	}

	// tessellation control shader
	GLuint tessEvaluation;
	if (tessEvaluationPath != NULL || tessEvaluationPath == "")
	{

		const char * teShaderCode = tessEvaluationCode.c_str();
		tessEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(tessEvaluation, 1, &teShaderCode, NULL);
		glCompileShader(tessEvaluation);

		// Check Tessellation Evaluation Shader
		glGetShaderiv(tessEvaluation, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(tessEvaluation, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> TessEvaluationShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(tessEvaluation, InfoLogLength, NULL, &TessEvaluationShaderErrorMessage[0]);
			printf("%s\n", &TessEvaluationShaderErrorMessage[0]);
		}

	}

	// shader Program
	ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertex);
	glAttachShader(ProgramID, fragment);
	if (geometryPath != NULL)
		glAttachShader(ProgramID, geometry);
	if (tessControlPath != NULL)
		glAttachShader(ProgramID, tessControl);
	if (tessEvaluationPath != NULL)
		glAttachShader(ProgramID, tessEvaluation);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath != NULL || geometryPath == "")
		glDeleteShader(geometry);
	if (tessControlPath != NULL || tessControlPath == "")
		glDeleteShader(tessControl);
	if (tessEvaluationPath != NULL || tessEvaluationPath == "")
		glDeleteShader(tessEvaluation);

}

void Shader::useProgram()
{
	glUseProgram(ProgramID);
	//execute queue of func
}

void Shader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const
{	
	glUniform3f(glGetUniformLocation(ProgramID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string & name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ProgramID, name.c_str()), 1, false, &value[0][0]);
}
