/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Camera.cpp
 *
 */

#include "Camera.h"


Camera::Camera(glm::vec3 pos, glm::vec3 up, glm::vec3 right) :
	frontVector(glm::vec3(0.0f, 0.0f, -1.0f)), position(pos), upVector(up), rightVector(right), yaw(-90.0f), pitch(0.0f) {}


glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + frontVector, upVector);
}

void Camera::updatePosition(char direction, float deltaTime)
{
	float cameraSpeed = speed * deltaTime;

	switch (direction)
	{
		// forward
		case 'f':
			position += frontVector * cameraSpeed;
			break;
		// backward
		case 'b':
			position -= frontVector * cameraSpeed;
			break;
		// left
		case 'l':
			position -= rightVector * cameraSpeed;
			break;
		// right
		case 'r':
			position += rightVector * cameraSpeed;
			break;
		// up
		case 'u':
			position += worldUp * cameraSpeed;
			break;
		// down
		case 'd':
			position -= worldUp * cameraSpeed;
			break;
	}
}

void Camera::processMouse(float xOffset, float yOffset)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	// compute camera angles
	yaw += xOffset;
	pitch += yOffset;

	// constraint camera angles
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateVectors(yaw, pitch);
}

void Camera::updateVectors(float yaw, float pitch)
{
	glm::vec3 front;

	// compute new camera direction vectors
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	frontVector = glm::normalize(front);
	rightVector = glm::normalize(glm::cross(frontVector, worldUp));
	upVector = glm::normalize(glm::cross(rightVector, frontVector));
}
