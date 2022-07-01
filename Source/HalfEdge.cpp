/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	HalfEdge.cpp
 *
 */

#include "HalfEdge.h"

heVertex::heVertex(glm::vec3 position)
{
	this->position = position;
}


float heVertex::distanceFromPlane(Plane plane)
{
	return (glm::dot(plane.normal, this->position) - plane.d);
}


Plane::Plane(const heVertex* vertex, glm::vec3 normal)
{
	this->d = glm::dot(vertex->position, normal);
	this->normal = normal;
}


HalfEdge::HalfEdge()
{
	this->tail = NULL;
	this->head = NULL;
	this->directionVector = glm::vec3(0.0f);
	this->previous = NULL;
	this->next = NULL;
	this->twin = NULL;
	this->face = NULL;
}

void HalfEdge::calculateDirectionVector()
{
	this->directionVector = glm::normalize(head->position - tail->position);
}


heFace::heFace(HalfEdge* edge, glm::vec3 normalVector)
{
	this->edge = edge;
	this->normal = normalVector;
}

bool heFace::isCoplanar(heFace* face)
{
	return (this->normal == face->normal);
}
