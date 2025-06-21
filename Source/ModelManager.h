/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	ModelManager.h
 *
 */

#pragma once
#pragma warning(disable:4996)

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <stdio.h>
#include <glm/glm.hpp>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "helpers/RootDir.h"
#include "Model.h"
#include "Hull.h"
#include "PlaneShape.h"
#include "Sphere.h"

namespace fs = std::filesystem;

/**
 * @brief Class for managing 3D models
 */
class ModelManager
{
public:
	// All models are stored here
	std::vector <Model*> models;

	ModelManager();

	/**
	 * @brief Destructor, deletes all stored models
	 */
	~ModelManager();

	/**
	 * @brief				Loads 3D model from .obj file, only files with vertex and normal positions
	 * @param modelFile		Path to file from which 3D model will be loaded
	 * @param name			Name of the model to be stored
	 * @param modelType		Type of the model shape, e.g. sphere, hull,...
	 * @return				Returns true on success, else returns false
	 */
	bool loadModel(fs::path modelFile, std::string name, ShapeType modelType, float radius);

	/**
	 * @brief		Deletes model from memory
	 * @param name	Name of the model to be deleted
	 */
	void deleteModel(std::string name);

	/**
	 * @brief			Gets pointer to the model
	 * @param name		Name of the model whose pointer to get
	 * @return			Pointer to the model of given name, NULL if model with given name doesn't exist
	 */
	Model* getModel(std::string name);

	/**
	 * @brief Creates GPU buffers and configurations of vertex attributes for all models 
	 */
	void createVAOs();

	/**
	 * @brief Inserts edge only if the same edge is not already in
	 * @param vertex0	Index of first vertex of the edge
	 * @param vertex0	Index of second vertex of the edge
	 * @param normal	Index of normal of the edge
	 * @param edges		Multimap into which to insert edge
	 */
	void insertEdge(unsigned int vertex0, unsigned int vertex1, unsigned int normal, 
		std::map<std::pair<unsigned int, unsigned int>, unsigned int> *edges);

private:
	void printAllocErrorMsg(std::string what);
};

#endif