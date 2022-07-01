/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Scene.h
 *
 */

#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Object.h"
#include "Camera.h"
#include "ModelManager.h"

class Scene
{
public:
	// Objects of the scene
	std::vector <Object*> objects;
	// Model manager of the scene
	ModelManager modelManager;

	/**
	 * @brief Constructor, initializes mainCamera and modelManager
	 */
	Scene();

	/**
	 * @brief Destructor, deletes all objects
	 */
	~Scene();

	/**
	 * @brief			Loads scene data from file and creates scene
	 * @param filename	File from which to load scene
	 * @return			Returns true if everything succeeded, false if something failed
	 */
	bool loadScene(fs::path filename);

	Camera* getCamera();

private:
	// Main Camera of the scene
	Camera camera;
};

#endif