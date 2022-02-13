#include "Transform.h"
#include <iostream>


Transform::Transform()
{
	translate_vec3 = glm::vec3(0.0f, 0.0f, 0.0f);
	rotate_vec3 = glm::vec3(0.0f, 0.0f, 0.0f);
	scale_vec3 = glm::vec3(1.0f, 1.0f, 1.0f);

    this->translate_mat4 = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
		translate_vec3.x, translate_vec3.y, translate_vec3.z, 1.0f
    );

	glm::mat4 rotateX = glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(glm::radians(rotate_vec3.x)), -sin(glm::radians(rotate_vec3.x)), 0.0f,
		0.0f, sin(glm::radians(rotate_vec3.x)), cos(glm::radians(rotate_vec3.x)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotateY = glm::mat4(
		cos(glm::radians(rotate_vec3.y)), 0.0f, sin(glm::radians(rotate_vec3.y)), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(glm::radians(rotate_vec3.y)), 0.0f, cos(glm::radians(rotate_vec3.y)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotateZ = glm::mat4(
		cos(glm::radians(rotate_vec3.z)), -sin(glm::radians(rotate_vec3.z)), 0.0f, 0.0f,
		sin(glm::radians(rotate_vec3.z)), cos(glm::radians(rotate_vec3.z)), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	rotate_mat4 = rotateY * rotateX * rotateZ;

	scale_mat4 = glm::mat4(
		scale_vec3.x, 0.0f, 0.0f, 0.0f,
		0.0f, scale_vec3.y, 0.0f, 0.0f,
		0.0f, 0.0f, scale_vec3.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}


Transform::~Transform()
{
}


void Transform::setTranslate(glm::vec3 translate)
{
	translate_vec3 = translate;

    translate_mat4 = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        translate.x, translate.y, translate.z, 1.0f
    );
}

void Transform::setRotate(glm::vec3 rotate) 
{
	rotate_vec3 = rotate;

    glm::mat4 rotateX = glm::mat4(
    	1.0f, 0.0f, 0.0f, 0.0f,
    	0.0f, cos(glm::radians(rotate.x)), -sin(glm::radians(rotate.x)), 0.0f,
    	0.0f, sin(glm::radians(rotate.x)), cos(glm::radians(rotate.x)), 0.0f,
    	0.0f, 0.0f, 0.0f, 1.0f
    );

	glm::mat4 rotateY = glm::mat4(
		cos(glm::radians(rotate.y)), 0.0f, sin(glm::radians(rotate.y)), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(glm::radians(rotate.y)), 0.0f, cos(glm::radians(rotate.y)), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	glm::mat4 rotateZ = glm::mat4(
		cos(glm::radians(rotate.z)), -sin(glm::radians(rotate.z)), 0.0f, 0.0f,
		sin(glm::radians(rotate.z)), cos(glm::radians(rotate.z)), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	rotate_mat4 = rotateY * rotateX * rotateZ;

}

void Transform::setScale(glm::vec3 scale)
{
	scale_vec3 = scale;

    scale_mat4 = glm::mat4(
    	scale.x, 0.0f, 0.0f, 0.0f,
    	0.0f, scale.y, 0.0f, 0.0f,
    	0.0f, 0.0f, scale.z, 0.0f,
    	0.0f, 0.0f, 0.0f, 1.0f
    );
}

glm::vec3 Transform::getTranslate()
{
	return translate_vec3;
}

glm::vec3 Transform::getRotate()
{
	return rotate_vec3;
}

glm::vec3 Transform::getScale()
{
	return scale_vec3;
}

glm::mat4 Transform::getModel()
{
    return (translate_mat4 * rotate_mat4 * scale_mat4);
}
