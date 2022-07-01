/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Hull.h
 *
 */

#pragma once

#ifndef HULL_H
#define HULL_H

#include <iostream>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Shape.h"
#include "HalfEdge.h"
#include "Object.h"


/**
 * @brief Structure representing projection interval
 */
struct interval {
	float min;		// minimal value of interval
	float max;		// maximal value of interval
};

class Hull : public Shape
{
public:
	std::vector<heVertex*> vertices;
	std::unordered_set<HalfEdge*> edges;
	std::vector<HalfEdge*> uniqueEdges;
	std::vector<heFace*> faces;

	Hull();
	virtual ~Hull();

	/**
	 * @brief Adds new vertex to the list of vertices
	 * @param position	Position of the vertex to be added
	 * @return			Whether creation of vertex succeeded or not
	 */
	bool addVertex(glm::vec3 position);

	/**
	 * @brief Merges faces that are divided by given edge, if faces are coplanar
	 * @param edge	Dividing edge
	 * @return		Whether faces were merged
	 */
	bool mergeFaceIfCoplanar(HalfEdge* edge);

	/**
	 * @brief Adds new face to the list of faces
	 * @param vertex1	Index of the first vertex of the face in vertex list
	 * @param vertex2	Index of the second vertex of the face in vertex list
	 * @param vertex3	Index of the third vertex of the face in vertex list
	 * @param normal	Normal of the face
	 * @return			Whether face was added
	 */
	bool addFace(int vertex1, int vertex2, int vertex3, glm::vec3 normal);

	/**
	 * @brief Free resources and add build unique edges vector
	 */
	void finalizeBuild();

	/**
	 * @brief Gets projection interval of the hull onto given axis
	 * @param axis	Axis onto which to project
	 * @return		Projection interval
	 */
	interval getProjectionInterval(glm::vec3 axis, const glm::mat4& transformationMatrix);

	/**
	 * @brief Finds hull vertex that is most extreme in given direction
	 * @param direction Direction vector in which to find most extreme point
	 * @param transformationMatrix Transformation matrix of the hull
	 * @return Position of the most extreme vertex in given direction
	 */
	glm::vec3 getSupport(const glm::vec3& direction, const glm::mat4& transformationMatrix);

	void calculateAttributes(Object* object) override;
private:

	/**
	 * @brief Structure for storing hash function
	 */ 
	struct vertexPairHash
	{
		size_t operator()(const std::pair<heVertex*, heVertex*> & pair) const;
	};

	// Used for finding edge twins during hull building
	std::unordered_map<std::pair<heVertex*, heVertex*>, HalfEdge*, vertexPairHash> vertexPairs;

	/**
	 * @brief Creates edges from given vertices and stores them in list of all edges of the hull
	 * @return Pointer to array of newly created edges or NULL on failure
	 */
	HalfEdge* createEdges(int& vertex1, int& vertex2, int& vertex3);

	/**
	 * @brief Checks if twin for given edge is known
	 */
	void checkForTwinEdge(HalfEdge* edge);

	/**
	 * @brief Finds twin of the given edge during hull build
	 * @param edge	Edge whose twin to find
	 * @return		Twin of given edge or NULL if twin was not found
	 */
	HalfEdge* findTwin(HalfEdge* edge);

	/**
	 * @brief Merge faces that are separated by given edge
	 * @param edge	Edge that separates faces to be merged
	 */
	void mergeFaces(HalfEdge* edge);

	/**
	 * @brief Helper function for computing subexpressions for inertia tensor
	 */
	inline void subexpressions(float w0, float w1, float w2, float& f1, float& f2, float& f3,
		float& g0, float& g1, float& g2);
};

#endif