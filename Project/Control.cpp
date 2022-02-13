#include "Control.h"
#include <iostream>

Control::Control()
{
}

Control::~Control()
{

}

void Control::initControl()
{
	// Initial position : on +Z
	position = glm::vec3(0, 3, 7);
	// Initial horizontal angle : toward -Z
	horizontalAngle = 3.14f;
	// Initial vertical angle : none
	verticalAngle = -0.6f;
	// Initial Field of View
	initialFoV = 45.0f;

	speed = 2.5f; // 3 units / second
	mouseSpeed = 0.005f;

	ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);
	ViewMatrix = glm::lookAt(position, position + direction, glm::vec3(0, 1, 0));

	glfwSetCursorPos(window, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //OBS GLFW_CURSOR_NORMAL & GLFW_CURSOR_DISABLE

	computeMatricesFromInputs();
}

glm::mat4 Control::getViewMatrix()
{
	return ViewMatrix;
}

glm::mat4 Control::getProjectionMatrix()
{
	return ProjectionMatrix;
}

glm::mat4 * Control::getViewMtx()
{
	return ViewMatrixP;
}

glm::mat4 * Control::getProjMtx()
{
	return ProjMatrixP;
}

void Control::computeMatricesFromInputs()
{
	
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(DISPLAY_WIDTH / 2 - xpos);
	verticalAngle += mouseSpeed * float(DISPLAY_HEIGHT / 2 - ypos);

	if (verticalAngle > 1.5f)
	{
		verticalAngle = 1.5f;
	}

	if (verticalAngle < -1.5f)
	{
		verticalAngle = -1.5f;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	direction = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	float finalSpeed = speed;

	// Sprint
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		finalSpeed = finalSpeed * 2.0;
	}
	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * finalSpeed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * finalSpeed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += right * deltaTime * finalSpeed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= right * deltaTime * finalSpeed;
	}
	// Move upward
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		position += up * deltaTime * finalSpeed;
	}
	// Move downward
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		position -= up * deltaTime * finalSpeed;
	}


	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

						   // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,             // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up                    // Head is up (set to 0,-1,0 to look upside-down)
	);

	ProjMatrixP = &ProjectionMatrix;
	ViewMatrixP = &ViewMatrix;

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

glm::vec3 Control::getPosition()
{
	return position;
}

glm::vec3 Control::getDirection()
{
	return direction;
}
