/**
* Bakalarska praca - Simualace pevnych teles
* VUT FIT, 2018/2019
*
* Autor:	Denis Leitner, xleitn02
* Subor:	Camera.h
*
*/

#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// vector pointing up in world coordinates
const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Speed of the camera
constexpr float speed = 8.0f;

// Sensitivity of the mouse
constexpr float mouseSensitivity = 0.1f;


class Camera
{
public:
	// Position of the camera in world coordinates
	glm::vec3 position;
	// Front direction vector of the camera
	glm::vec3 frontVector;
	// Up direction vector of the camera
	glm::vec3 upVector;
	// Right direction vector of the camera
	glm::vec3 rightVector;
	// Euler angles
	float yaw, pitch;

	/**
	 * @brief Constructor, creates camera
	 * @param pos	Position of the camera in world coordinates
	 * @param up	Up direction vector of the camera
	 * @param right	Right direction vector of the camera
	 */
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f));


	/**
	 * @brief Calculates view matrix and returns it
	 * @return	Calculated view matrix
	 */
	glm::mat4 getViewMatrix();

	/**
	 * @brief Updates camera position based on user input
	 * @param direction	Direction of the movement
	 					f - forward
	 					b - backward
	 					l - left
	 					r - right
						u - up
						d - down
	 * @param deltaTime	Time difference between last frame and current frame
	 */
	void updatePosition(char direction, float deltaTime);

	/**
	 * @brief Processes mouse input, computes new camera Euler angles
	 * @param xOffset	Differce in the mouse position on x-axis 
	 * @param yOffset	Differce in the mouse position on y-axis
	 */
	void processMouse(float xOffset, float yOffset);

private:
	/**
	 * @brief Updates camera direction vectors
	 * @param yaw	Yaw Euler angle of the camera
	 * @param pitch	Pitch Euler angle of the camera
	 */
	void updateVectors(float yaw, float pitch);
};

#endif

