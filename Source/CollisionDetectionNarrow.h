/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	CollisionDetectionNarrow.h
 *
 */

#pragma once

#ifndef COLLISION_DETECTION_NARROW_H
#define COLLISION_DETECTION_NARROW_H

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

#include "Hull.h"
#include "Object.h"
#include "PlaneShape.h"

constexpr float COEFFICIENT_OF_RESTITUTION = 0.5f;

struct CollisionData
{
	Object* object0;
	Object* object1;
	glm::vec3 collisionNormal;		// collision normal must point from object1 to object0
	glm::vec3 collisionPoint;
};

class CollisionDetectionNarrow
{
public:
	CollisionDetectionNarrow();

	/**
	 * @brief Decides which collision routine to choose according to shape of an object
	 * @return Whether objects collide
	 */
	bool checkCollision(CollisionData& collision, Object* object0, Object* object1);

private:

	struct Query
	{
		Object *object0;
		Object* object1;
		glm::vec3 axis;				// collision axis in world-space
		float separationDistance;
		Query();
	};

	struct FaceQuery : Query
	{
		unsigned int faceIndex;

		/**
		 * @brief Finds incident face of second colliding object
		 */
		heFace* findIncidentFace();
	};

	/**
	 * @brief Checks whether given objects collide, both objects are represented by convex hull
	 * @param[out] collision Informations about collision
	 * @param object0	First object to check for collision
	 * @param object1	Second object to check for collision
	 * @return			Whether objects collide or not
	 */
	bool checkCollisionHulls(CollisionData& collision, Object* object0, Object* object1);

	/**
	 * @brief Checks whether sphere objects collide
	 * @param[out] collision Informations about collision
	 * @param object0	First object to check for collision
	 * @param object1	Second object to check for collision
	 * @return			Whether objects collide or not
	 */
	bool checkCollisionSpheres(CollisionData& collision, Object* object0, Object* object1);

	/**
	 * @brief Checks whether plane and sphere collide
	 * @param[out] collision Informations about collision
	 * @param planeObject Plane object to check for collision
	 * @param sphereObject Sphere object to check for collision
	 * @return			Whether objects collide or not
	 */
	bool checkCollisionPlaneSphere(CollisionData& collision, Object* planeObject, Object* sphereObject);

	/**
	 * @brief Checks whether given objects collide, first object is hull, second is sphere
	 * @param[out] collision Informations about collision
	 * @param hullObject Hull object to check for collision
	 * @param sphereObject Sphere object to check for collision
	 * @return			Whether objects collide or not
	 */
	bool checkCollisionHullSphere(CollisionData& collision, Object* hullObject, Object* sphereObject);

	/**
	 * @brief Pushes objects out of collision, uses MTV
	 * @param object0 First object to push out of collision
	 * @param object1 Second object to push out of collision
	 * @param collisionNormal Collision normal pointing from object1 to object0
	 * @param seperationDistance Distance that separates both objects
	 */
	void pushObjectsOutOfCollision(Object* object0, Object* object1, const glm::vec3& collisionNormal, float seperationDistance);

	/**
	 * @brief Checks for overlap between 2 objects, as potential separating axes uses face normals
	 * @param object0 First object to be checked
	 * @param object1 Second object to be checked
	 * @return Information about collision 
	 */
	FaceQuery testFaceNormals(Object* object0, Object* object1);

	/**
	 * @brief Checks whether 2 intervals overlap
	 * @param interval0	First interval to check
	 * @param interval1	Second interval to check
	 * @return			Whether 2 intervals overlap or no
	 */
	bool overlap(interval interval0, interval interval1);

	/**
	 * @brief Performs Sutherland-Hodgman clipping of polygon against plane
	 * @param polygon	Polygon that we want to clip, represented as it's vertices
	 * @param plane		Plane against which to clip
	 * @param[out] out	Clipped polygon
	 */
	void SutherlandHodgman(const std::vector<heVertex>& polygon, const Plane& plane, std::vector<heVertex>& out);

	/**
	 * @brief Finds all collision points of given face-face collision query
	 * @param collisionQuery	Face-face collision query for which to find collision points
	 * @param[out] out			Found collision points
	 * @return					Whether collision could be created
	 */
	bool createFaceCollision(FaceQuery& collisionQuery, std::vector<glm::vec3>& out);
};

/**
 * @brief Checks if given vector is near zero vector(because of floating point arithmetics)
 * @param	Vector to be checked
 * @return	Whether vector is near zero length or not
 */
bool isZeroVector(glm::vec3 vector);

#endif

