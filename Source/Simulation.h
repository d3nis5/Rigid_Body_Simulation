/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Simulation.h
 *
 */

#pragma once

#ifndef SIMULATION_H
#define SIMULATION_H

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer.h"
#include "Scene.h"
#include "CollisionDetectionBroad.h"

constexpr float msPerUpdate = 0.01f;
constexpr float restingContactLimit = 0.3f;
constexpr float restingContactLimitHigher = 0.7f;
constexpr float restingDampingAngular = 0.7f;
constexpr float restingDampingAngularHigher = 0.85f;
constexpr float restingDampingLinear = 0.9f;
constexpr float restingDampingLinearHigher = 0.95f;

static Camera *mainCamera;

class Simulation
{
public:
	/**
	 * @brief Constructor, creates scene object and renderer object
	 */
	Simulation();

	/**
	 * @brief Destructor, deletes all resources
	 */
	~Simulation();

	/**
	 * @brief	Loads models, creates scene and initializes renderer
	 * @return	Returns true on success, false on failure
	 */
	bool initialize();

	/**
	 * @brief Runs simulation
	 */
	void run();

private:
	// width of the screen in pixels
	unsigned int screenWidth;
	// height of the screen in pixels
	unsigned int screenHeight;

	// simulated scene
	Scene *scene;
	// renderer of the simulation
	Renderer *renderer;
	// narrow-phase collision detector
	CollisionDetectionNarrow* collisionDetectorNarrow;
	// broad-phase collision detector, NULL if broad-phase is disabled
	CollisionDetectionBroad* collisionDetectorBroad;

	// indicates whether broad-phase collision is enabled
	bool broadPhaseEnabled;

	/** 
	 * @brief Updates position of the objects in scene
	 */
	void update();

	/**
	 * @brief Checks for collision between objects and resolves it; only narrow phase
	 */
	void checkCollisionNarrowPhase();

	/**
	 * @brief Checks for collision; broad and narrow phase
	 */
	void checkCollisionBroadPhase();

	/**
	 * @brief Applies impulses to all objects' linear and angular velocities accumulated throughout one update
	 */
	void applyImpulses();

	/**
	 * @brief Computes forces acting on all objects
	 */
	void computeForces();

	/**
	 * @brief Calculates impulses for collision response and stores them in objects' configuration
	 * @param collision Collision data
	 */
	void collisionResponse(const CollisionData& collision);

	/**
	 * @brief Collision response for static and dynamic object
	 * @param collision Collision data
	 */
	void collisionResponseStatic(const CollisionData& collision);

	/**
	 * @brief Collision response for dynamic objects
	 * @param collision Collision data
	 */
	void collisionResponseDynamic(const CollisionData& collision);

	/**
	 * @brief Applies damping to linear and angular velocity if object's velocities are low
	 * @param configuration Configuration of an object to which to apply damping
	 */
	void applyDamping(Object::Configuration& configuration);

	/**
	 * @brief Constructs skew-symmetric matrix for given vector
	 * @param vector Vector for which to construct skew-symmetric matrix
	 * @return Constructed skew-symmetric matrix
	 */
	glm::mat3 createSkewSymmetric(const glm::vec3& vector);
};

/**
 * @brief Processes user input from keyboard
 * @param window	Window whose input to process
 */
void processInput(GLFWwindow* window, float deltaTime);

/**
 * @brief Callback function, called when mouse is moved
 * @param window	Window whose mouse input to be captured
 * @param xpos		Actual position of mouse on x - axis
 * @param ypos		Actual position of mouse on y - axis
 */
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

/**
 * @brief Callback function for resizing viewport when window size is changed
 * @param window	Window whose size to resize
 * @param width		New width of viewport to be set
 * @param height	New height of viewport to be set
 */
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

#endif