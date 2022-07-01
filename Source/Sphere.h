/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Sphere.h
 *
 */

#pragma once

#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>

#include "Shape.h"
#include "Object.h"


class Sphere : public Shape
{
public:
	float radius;

	Sphere(float radius);

	/**
	 * @brief Finds sphere point that is most extreme in given direction
	 * @param direction Direction vector in which to find most extreme point
	 * @param spherePosition Position of sphere center
	 * @return Position of the most extreme point in given direction
	 */
	glm::vec3 getSupport(const glm::vec3& direction, const glm::vec3& spherePosition);

	void calculateAttributes(Object* object) override;

	/**
	 * @brief Finds projection interval of a sphere
	 * @param axis Axis on which to find projection interval
	 * @param sphereCenter Sphere center of which to find projection interval
	 */
	struct interval getProjectionInterval(const glm::vec3& axis, const glm::vec3& sphereCenter);
};

#endif
