#pragma once

#include "Common.h"

#include <GLM\glm.hpp>

class Transform
{
public:
	Transform();
	~Transform();

	void setTranslate(glm::vec3 translate);
	void setRotate(glm::vec3 rotate);
	void setScale(glm::vec3 scale);

	glm::vec3 getTranslate();
	glm::vec3 getRotate();
	glm::vec3 getScale();

    glm::mat4 getModel();

private:
	glm::vec3 translate_vec3;
	glm::vec3 rotate_vec3;
	glm::vec3 scale_vec3;

	glm::mat4 translate_mat4;
	glm::mat4 rotate_mat4;
	glm::mat4 scale_mat4;
};