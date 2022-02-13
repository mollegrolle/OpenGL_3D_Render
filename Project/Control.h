#pragma once

#include "Common.h"

class Control
{
public:
	Control();
	~Control();

	void initControl();

	void computeMatricesFromInputs();
	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::mat4 *getViewMtx();
	glm::mat4 *getProjMtx();

private:

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::mat4 *ViewMatrixP;
	glm::mat4 *ProjMatrixP;

	// Initial position : on +Z
	glm::vec3 position;
	// Initial direction
	glm::vec3 direction;
	// Initial horizontal angle : toward -Z
	float horizontalAngle;
	// Initial vertical angle : none
	float verticalAngle;
	// Initial Field of View
	float initialFoV;

	float speed; // 3 units / second
	float mouseSpeed;

};