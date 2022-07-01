/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Model.h
 *
 */

#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <glm/glm.hpp>

#include "Shape.h"

constexpr auto EPSILON = 0.00001f;

// forward declarations
class Hull;
class Object;

/**
 * @brief Structure representing vertex, storing it's position and normal
 */
typedef struct vertex {
	/// position of the vertex
	glm::vec3 position;
	/// normal of the vertex
	glm::vec3 normal;

	bool operator<(const vertex that) const
	{
		return memcmp((void*)this, (void*)&that, sizeof(struct vertex)) > 0;
	};
} Vertex;

/**
 * @brief Class representing 3D model
 */
class Model
{
public:
	// Name of the model
	std::string modelName;

	// shape representing this model
	Shape* shape;

	// Buffer for storing vertex data 
	// !!! after data are sent to GPU it is deleted !!!
	std::vector <Vertex> vertexBuffer;

	// Buffer for storing indices to vertices
	// !!! after data are sent to GPU it is deleted !!!
	std::vector<unsigned int> indexBuffer;

	// all model vertices
	// !!! after data are sent to GPU it is deleted !!!
	std::vector<glm::vec3> vertices;

	// all model normals
	// !!! after data are sent to GPU it is deleted !!!
	std::vector<glm::vec3> normals;

	// number of vertices to draw
	unsigned int verticesCount;

	// vertex array object of the model
	unsigned int vao;

	Model(std::string name);

	~Model();
};

#endif