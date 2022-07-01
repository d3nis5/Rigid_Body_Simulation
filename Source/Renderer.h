/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Renderer.h
 *
 */

#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Scene.h"


extern unsigned int SCREEN_WIDTH, SCREEN_HEIGHT;

/**
 * Class for rendering graphics
 */
class Renderer
{
public:
	GLFWwindow* window;

	Renderer();
	~Renderer();

	/**
	 * @brief Initialize renderer
	 * @param vertexFile Path to vertex shader file
	 * @param fragmentFile Path to fragment shader file
	 */
	bool initialize(fs::path vertexFile, fs::path fragmentFile);

	/**
	 * @brief Sets scene to be rendered
	 * @param scene Scene to be rendered
	 */
	void setScene(Scene *scene);

	/**
	 * @brief Sets camera for viewing
	 * @param camera Camera to be set
	 */
	void setCamera(Camera *camera);
	Shader* getShader();

	/**
	 * @brief Draws all objects in scene
	 */
	void draw();

private:
	unsigned int screenWidth;
	unsigned int screenHeight;
	Camera *camera;
	Scene *scene;
	Shader *shader;
	glm::vec4 backgroundColor;
};

#endif 