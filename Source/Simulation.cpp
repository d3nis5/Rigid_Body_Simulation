/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Simulation.cpp
 *
 */

#include "Simulation.h"

const glm::vec3 GRAVITY = glm::vec3(0.0f, -9.8f, 0.0f);

Simulation::Simulation()
{
	screenWidth = 800;
	screenHeight = 600;
	broadPhaseEnabled = false;
	collisionDetectorBroad = NULL;
	collisionDetectorNarrow = NULL;

	try
	{
		scene = new Scene();
		renderer = new Renderer();
		collisionDetectorNarrow = new CollisionDetectionNarrow();

		// set scene to renderer
		renderer->setScene(scene);
	}
	catch (std::bad_alloc & ba)
	{
		std::cout << "Could not allocate memory: " << ba.what() << std::endl;
		throw;
	}
}


Simulation::~Simulation()
{
	if (renderer != NULL)
	{
		delete renderer;
	}
	if (scene != NULL)
	{
		delete scene;
	}
	if (collisionDetectorNarrow != NULL)
	{
		delete collisionDetectorNarrow;
	}
	if (collisionDetectorBroad != NULL)
	{
		delete collisionDetectorBroad;
	}
}

bool Simulation::initialize()
{
	int sceneNumber;

	std::cout << "Enter scene number: ";
	std::cin >> sceneNumber;

	std::string sceneName = "Scenes/scene_";
	sceneName = sceneName + std::to_string(sceneNumber);

	fs::path rootDir = fs::u8path(ROOT_DIR);
	fs::path scenePath = rootDir;
	fs::path vertexShaderPath = rootDir;
	fs::path fragmentShaderPath = rootDir;

	fs::path sceneNamePath = fs::path(sceneName);
	scenePath += sceneNamePath;

	if (!scene->loadScene(scenePath))
		return false;

	std::cout << std::endl << "Enable broad-phase collision detection? (y/n): ";
	char broadPhaseInput;
	std::cin >> broadPhaseInput;

	if (broadPhaseInput == 'y')
	{
		broadPhaseEnabled = true;
		try
		{
			// simulation's narrow-phase collision detector will be used by broad-phase collision detector
			collisionDetectorBroad = new CollisionDetectionBroad(collisionDetectorNarrow);
		}
		catch (std::bad_alloc)
		{
			return false;
		}

		collisionDetectorNarrow = NULL;
		std::cout << "Broad-phase collision detection is enabled" << std::endl;
	}
	else
	{
		std::cout << "Broad-phase collision detection is disabled" << std::endl;
	}

	fs::path vertex = fs::path("Shaders/vertex.vert");
	fs::path fragment = std::filesystem::path("Shaders/fragment.frag");

	vertexShaderPath += vertex;
	fragmentShaderPath += fragment;

	if (!renderer->initialize(vertexShaderPath, fragmentShaderPath))
	{
		std::cout << "Could not initialize renderer" << std::endl;
		return false;
	}

	// create VAOs for all models
	scene->modelManager.createVAOs();
	mainCamera = scene->getCamera();

	// hide mouse cursor
	glfwSetInputMode(renderer->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// set callback for resizing viewport
	glfwSetFramebufferSizeCallback(renderer->window, framebufferSizeCallback);
	// set callback for when mouse is moved
	glfwSetCursorPosCallback(renderer->window, mouseCallback);

	// recompute initial world-space inverse inertia tensor for every object
	for (auto & object : scene->objects)
	{
		if (object->density != INFINITY)
			object->computeInverseWorldInertiaTensor();
		else
		{
			if (broadPhaseEnabled)
			{
				if (!collisionDetectorBroad->insertStaticObject(object))
				{
					std::cout << "Static object \"" << object->objectName << "\" couldn't be inserted into grid\n";
					return false;
				}
			}
		}
	}

	return true;
}

void Simulation::run()
{
	renderer->setCamera(mainCamera);

	Shader *myShader = renderer->getShader();
	myShader->use();

	glfwSetTime(0.0);

	float lastTime = (float)glfwGetTime();
	float lastFPS = 0.0;

	float accumulator = 0.0f;

	computeForces();

	// main loop
	while (!glfwWindowShouldClose(renderer->window))
	{
		float currentTime = (float)glfwGetTime();
		float frameTime = currentTime - lastTime;

		lastTime = currentTime;

		if (currentTime - lastFPS >= 1.0f)
		{
			char fps[64];
			snprintf(fps, 64, "Rigid Body Simulation  %f FPS", 1 / frameTime);

			glfwSetWindowTitle(renderer->window, fps);
			lastFPS = currentTime;
		}

		processInput(renderer->window, frameTime);

		accumulator += frameTime;

		static int counter = 0;
		static double acc = 0.0;

		while (accumulator >= msPerUpdate)
		{
			double start = glfwGetTime();
			update();
			double end = glfwGetTime();

			counter++;
			acc += (end - start) * 1000.0;

			accumulator -= msPerUpdate;
		}

		renderer->draw();

		glfwSwapBuffers(renderer->window);
		glfwPollEvents();
	}
}

void Simulation::update()
{
	std::vector <Object*>::iterator it;

	for (it = scene->objects.begin(); it != scene->objects.end(); it++)
	{
		Object *object = *it;
		Object::Configuration& configuration = object->configuration;

		// static object
		if (object->density == INFINITY)
			continue;

		glm::vec3 acceleration = configuration.force * object->inverseMass;

		configuration.velocityVector += msPerUpdate * acceleration;
		configuration.angularVelocity = configuration.inverseWorldInertiaTensor * configuration.angularMomentum;

		float damping = 1.0f / (1.0f + msPerUpdate * 0.25f);
		configuration.velocityVector *= damping;
		configuration.angularMomentum *= damping;

		configuration.position += msPerUpdate * configuration.velocityVector;
		configuration.rotation += msPerUpdate * createSkewSymmetric(configuration.angularVelocity) * configuration.rotation;

		object->reorthogonalizeRotationMatrix();
		object->computeInverseWorldInertiaTensor();

		if (broadPhaseEnabled)
		{
			object->aabb->recomputeAABB(object);
		}
	}

	if (broadPhaseEnabled)
	{
		checkCollisionBroadPhase();
	}
	else
	{
		checkCollisionNarrowPhase();
	}
	applyImpulses();
}

void Simulation::checkCollisionNarrowPhase()
{
	Object* object0;
	Object* object1;

	const auto& objects = scene->objects;
	unsigned numberOfObjects = objects.size();

	for (unsigned i = 0; i < numberOfObjects - 1; i++)
	{
		object0 = objects[i];

		for (unsigned j = i + 1; j < numberOfObjects; j++)
		{
			object1 = objects[j];

			if (object0->mass == INFINITY && object1->mass == INFINITY)
				// static objects
				continue;

			CollisionData collision;

			if (collisionDetectorNarrow->checkCollision(collision, object0, object1))
			{
				collisionResponse(collision);
			}
		}
	}
}

void Simulation::checkCollisionBroadPhase()
{
	const auto& objects = scene->objects;
	unsigned int i = 0;

	for (auto & object : objects)
	{
		// skip static object, it is already in grid
		if (object->mass == INFINITY)
			continue;

		// all actual collisions of object are stored here
		std::vector<CollisionData> collisions;

		if (!collisionDetectorBroad->check(object, collisions))
			continue;

		for (auto & collision : collisions)
		{
			collisionResponse(collision);
		}
		i += collisions.size();
	}

	collisionDetectorBroad->clearGrid();
}

void Simulation::applyImpulses()
{
	const auto& objects = scene->objects;

	for (unsigned i = 0; i < objects.size(); i++)
	{
		Object::Configuration& configuration = objects[i]->configuration;

		// update configuration by impulses
		configuration.angularVelocity = configuration.inverseWorldInertiaTensor * configuration.angularMomentum;
		configuration.velocityVector += configuration.velocityAccumulator;
		configuration.velocityAccumulator = glm::vec3(0.0f);
	}
}

void Simulation::computeForces()
{
	for (unsigned i = 0; i < scene->objects.size(); i++)
	{
		Object* object = scene->objects[i];
		Object::Configuration& configuration = object->configuration;

		configuration.force = GRAVITY * object->mass;
		configuration.torque = glm::vec3(0.0f);
	}
}

void Simulation::collisionResponse(const CollisionData& collision)
{
	if (collision.object0->mass == INFINITY || collision.object1->mass == INFINITY)
	{
		// one of the objects is static
		collisionResponseStatic(collision);
	}
	else
	{
		// both objects are dynamic
		collisionResponseDynamic(collision);
	}
}

void Simulation::collisionResponseStatic(const CollisionData& collision)
{
	Object* object0 = collision.object0;
	Object* object1 = collision.object1;
	glm::vec3 collisionNormal = collision.collisionNormal;
	Object::Configuration& configuration0 = object0->configuration;
	Object::Configuration& configuration1 = object1->configuration;
	
	Object* object;
	glm::mat4 transformMatrix;

	// only dynamic object will be used in calculation
	if (object0->mass == INFINITY)
	{
		object = object1;
		transformMatrix = collision.object1->getModelMatrix();
		collisionNormal = -collisionNormal;
	}
	else
	{
		object = object0;
		transformMatrix = collision.object0->getModelMatrix();
	}

	Object::Configuration& configuration = object->configuration;
	constexpr float restitution = -(1.0f + COEFFICIENT_OF_RESTITUTION);
	glm::vec3 centerOfMass = transformMatrix * glm::vec4(object->centerOfMass, 1.0f);
	glm::vec3 point = collision.collisionPoint;

	glm::vec3 pointImpulse = glm::vec3(0.0f);

	// vector from center of mass of an object to collision point
	glm::vec3 r0 = point - centerOfMass;

	glm::vec3 pointVelocity = configuration.velocityVector + glm::cross(configuration.angularVelocity, r0);
	float relativeVelocity = glm::dot(pointVelocity, collisionNormal);

	float subexpression = glm::dot(glm::cross(configuration.inverseWorldInertiaTensor * glm::cross(r0, collisionNormal), r0), collisionNormal);
	float pointImpulseNumerator = restitution * relativeVelocity;
	float pointImpulseDenominator = object->inverseMass + subexpression;
	float pointImpulseMagnitude = pointImpulseNumerator / pointImpulseDenominator;
	pointImpulse = pointImpulseMagnitude * collisionNormal;

	configuration.angularMomentum += glm::cross(r0, pointImpulse);
	configuration.velocityAccumulator += (pointImpulse) * object->inverseMass;

	applyDamping(configuration);
}

void Simulation::collisionResponseDynamic(const CollisionData& collision)
{
	Object* object0 = collision.object0;
	Object* object1 = collision.object1;
	glm::vec3 collisionNormal = collision.collisionNormal;
	Object::Configuration& configuration0 = object0->configuration;
	Object::Configuration& configuration1 = object1->configuration;
	glm::mat4 transformMatrix0 = object0->getModelMatrix();
	glm::mat4 transformMatrix1 = object1->getModelMatrix();

	constexpr float restitution = -(1.0f + COEFFICIENT_OF_RESTITUTION);
	float impulseDenominator = object0->inverseMass + object1->inverseMass;

	glm::vec3 centerOfMassObj0 = transformMatrix0 * glm::vec4(object0->centerOfMass, 1.0f);
	glm::vec3 centerOfMassObj1 = transformMatrix1 * glm::vec4(object1->centerOfMass, 1.0f);

	glm::vec3 point = collision.collisionPoint;

	// vectors from center of mass of an object to collision point
	glm::vec3 r0 = point - centerOfMassObj0;
	glm::vec3 r1 = point - centerOfMassObj1;

	glm::vec3 pointVelocity0 = configuration0.velocityVector + glm::cross(configuration0.angularVelocity, r0);
	glm::vec3 pointVelocity1 = configuration1.velocityVector + glm::cross(configuration1.angularVelocity, r1);
	float relativeVelocity = glm::dot(pointVelocity0 - pointVelocity1, collisionNormal);

	auto subexpression0 = glm::cross(configuration0.inverseWorldInertiaTensor * glm::cross(r0, collisionNormal), r0);
	auto subexpression1 = glm::cross(configuration1.inverseWorldInertiaTensor * glm::cross(r1, collisionNormal), r1);

	float pointImpulseNumerator = restitution * relativeVelocity;
	float pointImpulseDenominator = impulseDenominator + glm::dot(subexpression0 + subexpression1, collisionNormal);
	float pointImpulseMagnitude = pointImpulseNumerator / pointImpulseDenominator;
	glm::vec3 pointImpulse = pointImpulseMagnitude * collisionNormal;

	configuration0.angularMomentum += glm::cross(r0, pointImpulse);
	configuration1.angularMomentum -= glm::cross(r1, pointImpulse);

	configuration0.velocityAccumulator += pointImpulse * object0->inverseMass;
	configuration1.velocityAccumulator -= pointImpulse * object1->inverseMass;

	applyDamping(configuration0);
	applyDamping(configuration1);
}

void Simulation::applyDamping(Object::Configuration& configuration)
{
	float linearVelocity = glm::dot(configuration.velocityVector, configuration.velocityVector);
	float angularVelocity = glm::dot(configuration.angularVelocity, configuration.angularVelocity);

	if (linearVelocity < restingContactLimit && angularVelocity < restingContactLimit)
	{
		configuration.velocityVector *= restingDampingLinear;
		configuration.angularMomentum *= restingDampingAngular;
	}
	else if (linearVelocity < restingContactLimitHigher && angularVelocity < restingContactLimitHigher)
	{
		configuration.velocityVector *= restingDampingLinearHigher;
		configuration.angularMomentum *= restingDampingAngularHigher;
	}
}


glm::mat3 Simulation::createSkewSymmetric(const glm::vec3& vector)
{
	glm::mat3 skewSymmetric = glm::mat3(0.0f);

	skewSymmetric[1][0] = -vector.z;
	skewSymmetric[2][0] = vector.y;
	skewSymmetric[0][1] = vector.z;
	skewSymmetric[2][1] = -vector.x;
	skewSymmetric[0][2] = -vector.y;
	skewSymmetric[1][2] = vector.x;

	return skewSymmetric;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;

	std::cout << "New screen size " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << std::endl;
}

void processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		(*mainCamera).updatePosition('f', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		(*mainCamera).updatePosition('b', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		(*mainCamera).updatePosition('l', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		(*mainCamera).updatePosition('r', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		(*mainCamera).updatePosition('u', deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		(*mainCamera).updatePosition('d', deltaTime);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	// flag if this function is called for the first time
	static bool first = true;
	// last position of the mouse on x-axis
	static float lastX = (float)SCREEN_WIDTH / 2.0f;
	// last position of the mouse on y-axis
	static float lastY = (float)SCREEN_HEIGHT / 2.0f;

	// this ensures that camera doesn't change direction suddenly, when the function is called for the first time
	if (first)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		first = false;
	}

	// difference in the mouse position
	float xOffset = (float)xpos - lastX;
	float yOffset = lastY - (float)ypos;

	lastX = (float)xpos;
	lastY = (float)ypos;

	(*mainCamera).processMouse(xOffset, yOffset);
}
