/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Sphere.cpp
 *
 */

#include "Sphere.h"

const float PI = 3.14159f;

Sphere::Sphere(float radius)
{
	this->type = sphere;
	this->radius = radius;
}

glm::vec3 Sphere::getSupport(const glm::vec3& direction, const glm::vec3& spherePosition)
{
	return spherePosition + direction * this->radius;
}

void Sphere::calculateAttributes(Object* object)
{
	float volume = 4.0f/3.0f * PI * radius * radius * radius;	// 4/3 PI * r^3

	object->centerOfMass = glm::vec3(0.0f);
	object->mass = volume * object->density;
	object->inverseMass = 1.0f / object->mass;

	glm::mat4 inertia = glm::mat4(2.0f / 5.0f * object->mass * radius * radius);
	object->inverseBodyInertiaTensor = glm::inverse(inertia);
}

interval Sphere::getProjectionInterval(const glm::vec3& axis, const glm::vec3& sphereCenter)
{
	float centerProjection = glm::dot(axis, sphereCenter);

	interval result = { centerProjection - radius, centerProjection + radius };
	return result;
}
