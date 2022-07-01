/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Shader.cpp
 *
 */

#include "Shader.h"


Shader::Shader(fs::path vertexFile, fs::path fragmentFile)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexShaderF;		// file in which vertex shader is saved
	std::ifstream fragmentShaderF;		// file in which fragment shader is saved


	// fstream objects can throw exceptions
	vertexShaderF.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	fragmentShaderF.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	try
	{
		// open files
		vertexShaderF.open(vertexFile);
		fragmentShaderF.open(fragmentFile);

		std::stringstream vertexStream, fragmentStream;
		// read files into streams
		vertexStream << vertexShaderF.rdbuf();
		fragmentStream << fragmentShaderF.rdbuf();
		vertexShaderF.close();
		fragmentShaderF.close();

		// convert string stream into string
		vertexCode = vertexStream.str();
		fragmentCode = fragmentStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "Failed to read shader file!" << std::endl;
	}

	const char* v_code = vertexCode.c_str();
	const char* f_code = fragmentCode.c_str();

	unsigned int vertex, fragment;

	// create and compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_code, NULL);
	glCompileShader(vertex);
	checkShaderCompilation(vertex, "Vertex");

	// create and compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_code, NULL);
	glCompileShader(fragment);
	checkShaderCompilation(vertex, "Fragment");

	// create shader program
	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	checkProgramLinking(id);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setUniformMat4(glm::mat4 matrix, const char* uniformName)
{
	unsigned int location = glGetUniformLocation(id, uniformName);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformMat3(glm::mat3 matrix, const char* uniformName)
{
	unsigned int location = glGetUniformLocation(id, uniformName);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformVec3(glm::vec3 vector, const char* uniformName)
{
	unsigned int location = glGetUniformLocation(id, uniformName);
	glUniform3fv(location, 1, glm::value_ptr(vector));
}

void Shader::checkShaderCompilation(unsigned int shader, std::string type)
{
	char infoLog[512];
	int success = 0;

	// check for compilation errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		// compilation failed, get info log
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << type << " shader compilation failed!\n" << infoLog << std::endl;
	}
	
}

void Shader::checkProgramLinking(unsigned int program)
{
	char infoLog[512];
	int success = 0;

	// check for linking errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		// linking failed, get info log
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Shader program linking failed!\n" << infoLog << std::endl;
	}
}