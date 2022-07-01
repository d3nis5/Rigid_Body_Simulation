/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	HalfEdge.h
 *
 */

#pragma once

#ifndef HALF_EDGE_H
#define HALF_EDGE_H

#include <glm/glm.hpp>


struct Plane
{
	float d;	// dot product of plane normal and any point on plane
	glm::vec3 normal;

	Plane(const struct heVertex* vertex, glm::vec3 normal);
};

 /**
  * @brief Half edge vertex representation
  */
struct heVertex
{
	heVertex(glm::vec3 position);
	glm::vec3 position;

	float distanceFromPlane(Plane plane);
};

struct HalfEdge
{
	heVertex* tail;			// tail vertex
	heVertex* head;			// head vertex

	glm::vec3 directionVector;	// direction vector of the edge

	HalfEdge* previous;	// previous edge in face
	HalfEdge* next;		// next edge in face
	HalfEdge* twin;		// twin edge

	struct heFace* face;	// face into which edge belongs

	HalfEdge();
	void calculateDirectionVector();
};

struct heFace
{
	HalfEdge* edge;
	glm::vec3 normal;

	heFace(HalfEdge* edge, glm::vec3 normalVector);

	/**
	 * @brief Finds out if face is coplanar with given face
	 * @param face	Face which to check
	 * @return		Whether faces are coplanar
	 */
	bool isCoplanar(heFace* face);
};

#endif