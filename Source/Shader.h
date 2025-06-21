/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Shader.h
 *
 */

#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * @brief Class for working with shaders
 */
class Shader
{
public:
	// Shader program ID
	unsigned int id;

	/**
	 * @brief Constructor, reads shader code from file and builds shader
	 * @param vertexFile	Path to the vertex shader file
	 * @param fragmentFile	Path to the fragment shader file
	 */
	Shader(fs::path vertexFile, fs::path fragmentFile);

	/**
	 * @brief Activate the shader
	 */
	void use();

	/**
	 * @brief Sets 4x4 matrix uniform
	 * @param matrix		Matrix to set as uniform
	 * @param uniformName	Name of the uniform in shader
	 */
	void setUniformMat4(glm::mat4 matrix, const char* uniformName);

	/**
	 * @brief Sets 3x3 matrix uniform
	 * @param matrix Matrix to set as uniform
	 * @param uniformName Name of the set uniform
	 */
	void setUniformMat3(glm::mat3 matrix, const char* uniformName);
	/**
	 * @brief Sets vector 3 uniform
	 * @param vector Vector to set as uniform
	 * @param uniformName Name of the set uniform
	 */
	void setUniformVec3(glm::vec3 vector, const char* uniformName);
private:
	/**
	 * @brief Checks if there were any errors during shader compilation
	 * @param shader	Shader whose compilation errors to check
	 * @param type		Type of the shader whose compilation to check
	 */
	void checkShaderCompilation(unsigned int shader, std::string type);

	/**
	 * @brief Checks if there were any errors during shader program linking
	 * @param program	Shader program whose linking errors to check
	 */
	void checkProgramLinking(unsigned int program);
};

#endif