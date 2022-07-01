/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Renderer.cpp
 *
 */

#include "Renderer.h"


Renderer::Renderer()
{
	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;
	backgroundColor = glm::vec4(0.18f, 0.3f, 0.3f, 1.0f);
}


Renderer::~Renderer()
{
	if (shader != NULL)
	{
		delete shader;
	}
	if (window != NULL)
	{
		glfwTerminate();
	}
}

bool Renderer::initialize(fs::path vertexFile, fs::path fragmentFile)
{
	// initialize GLFW
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "Failed to initialize 'glfw'!" << std::endl;
		return false;
	}

	// configure GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create window
	window = glfwCreateWindow(screenWidth, screenHeight, "Rigid Body Simulation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!" << std::endl;
		return false;
	}

	try
	{
		shader = new Shader(vertexFile, fragmentFile);
	}
	catch (std::bad_alloc & ba)
	{
		std::cout << "Couldn't allocate memory for shader: " << ba.what() << std::endl;
		return false;
	}

	// enable depth testing
	glEnable(GL_DEPTH_TEST);

	// enable backface culling
	glEnable(GL_CULL_FACE);

	// drawing only lines
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	return true;
}

void Renderer::setScene(Scene *s)
{
	scene = s;
}

void Renderer::setCamera(Camera *c)
{
	camera = c;
}


Shader* Renderer::getShader()
{
	return shader;
}


void Renderer::draw()
{
	std::vector <Object*>::iterator it;

	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// view matrix
	glm::mat4 view = camera->getViewMatrix();

	// projection matrix
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

	// set transformation matrices
	shader->setUniformMat4(view, "view");
	shader->setUniformMat4(projection, "projection");

	// iterate through all objects of the scene and draw them
	for (it = scene->objects.begin(); it != scene->objects.end(); it++)
	{
		Object *obj = *it;

		// model matrix
		glm::mat4 model = obj->getModelMatrix();
		// matrix for normal rotation
		glm::mat3 normalMatrix = obj->configuration.rotation;

		shader->setUniformMat4(model, "model");
		shader->setUniformMat3(normalMatrix, "normalMatrix");
		shader->setUniformVec3(obj->color, "color");
		// bind VAO of drawn model
		glBindVertexArray(obj->model->vao);
		
		glDrawElements(GL_TRIANGLES, obj->model->verticesCount, GL_UNSIGNED_INT, 0);
	}
}
