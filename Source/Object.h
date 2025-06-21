/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Object.h
 *
 */

#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

#include "Hull.h"
#include "Model.h"
#include "Sphere.h"

class AABB
{
public:
	glm::vec3 min;	// minimal coordinates along each axis
	glm::vec3 max;	// maximal coordinates along each axis

	/**
	 * @brief Creates new AABB for given object
	 * @param object Object for which to create AABB
	 */
	AABB(Object* object);

	/**
	 * @brief Computes AABB for given Object
	 * @param object Object for which to recompute AABB
	 */
	void recomputeAABB(Object* object);

private:
	/**
	 * @brief Assigns value to min if value is lesser than min, to max if value is greater than max
	 */
	void assignMinMax(float value, float& min, float& max);
};

/**
 * @brief Class representing object
 */
class Object
{
public:
	// name of the object
	std::string objectName;
	// pointer to the model(mesh)
	Model *model;
	// axis aligned bounding box of an object
	AABB *aabb;
	// color of an object
	glm::vec3 color;
	// density of an object
	float density;
	float mass;
	// 1/mass
	float inverseMass;
	glm::vec3 centerOfMass;

	// inverse inertia tensor in body-space
	glm::mat3 inverseBodyInertiaTensor;

	struct Configuration
	{
		glm::vec3 position;
		glm::mat3 rotation;

		glm::vec3 velocityAccumulator;
		glm::vec3 velocityVector;
		glm::vec3 angularMomentum;

		glm::vec3 force;
		glm::vec3 torque;

		// inverse inertia tensor in world-space, must be recomputed every frame
		glm::mat3 inverseWorldInertiaTensor;
		glm::vec3 angularVelocity;

		Configuration();
	} configuration;

	struct ObjectInit
	{
		std::string objectName;
		Model* model;
		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 initialVelocity;
		float density;
		ObjectInit();
	};

	Object(ObjectInit initValues);
	~Object();

	glm::mat4 getModelMatrix();
	glm::mat3 getRotationMatrix();
	glm::vec3 getPosition();
	void computeInverseWorldInertiaTensor();
	void reorthogonalizeRotationMatrix();
private:
	glm::mat3 constructRotationMatrix(glm::vec3 eulerAngles);
};

#endif