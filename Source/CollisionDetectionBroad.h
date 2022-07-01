/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	CollisionDetectionBroad.h
 *
 */

#pragma once

#ifndef COLLISION_DETECTION_BROAD_H
#define COLLISION_DETECTION_BROAD_H

#include "Grid.h"
#include "CollisionDetectionNarrow.h"

class CollisionDetectionBroad
{
public:
	CollisionDetectionBroad(CollisionDetectionNarrow* collisionDetector);
	~CollisionDetectionBroad();

	/**
	 * @brief Checks given object for collision with other objects and inserts it into grid
	 * @param object Object for which to check collisions
	 * @param[out] collisions Collisions found by collision detector
	 * @return whether any check was done
	 */
	bool check(Object* object, std::vector<CollisionData>& collisions);

	/**
	 * @brief Checks whether 2 AABBs overlap
	 * @param aabb0 First AABB to check
	 * @param aabb1 Second AABB to check
	 * @return Whether 2 AABBs overlap
	 */
	bool checkCollisionAABBs(AABB* aabb0, AABB* aabb1);

	/**
	 * @brief Removes all dynamic objects from grid cells
	 */
	void clearGrid();

	/**
	 * @brief Inserts static object into grid
	 * @param object Object to insert into grid
	 * @return Whether object was inserted
	 */
	bool insertStaticObject(Object* object);
private:
	Grid* grid;
	CollisionDetectionNarrow* collisionDetectorNarrow;
};

#endif