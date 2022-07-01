/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	CollisionDetectionBroad.cpp
 *
 */

#include "CollisionDetectionBroad.h"

CollisionDetectionBroad::CollisionDetectionBroad(CollisionDetectionNarrow* collisionDetector)
{
	collisionDetectorNarrow = collisionDetector;

	try
	{
		grid = new Grid();
		if (grid->cells == NULL)
		{
			// memory for grid cells couldn't be allocated
			delete grid;
			grid = NULL;
		}
	}
	catch (std::bad_alloc)
	{
		grid = NULL;
	}
}


CollisionDetectionBroad::~CollisionDetectionBroad()
{
	if (grid != NULL)
		delete grid;

	if (collisionDetectorNarrow != NULL)
		delete collisionDetectorNarrow;
}

bool CollisionDetectionBroad::check(Object* object, std::vector<CollisionData>& collisions)
{
	// indices to grid cell array of min and max corners of AABB
	glm::uvec3 minIndices;
	glm::uvec3 maxIndices;

	// indices of min corner of an object's AABB in the grid
	if (!grid->mapPositionToIndices(object->aabb->min, minIndices))
	{
		// can't be inserted into grid
		return false;
	}

	// indices of max corner of an object's AABB in the grid
	if (!grid->mapPositionToIndices(object->aabb->max, maxIndices))
	{
		// can't be inserted into grid
		return false;
	}

	std::unordered_set<Object*> potentialCollisionObjects;

	// insert object into every cell it occupies
	for (unsigned x = minIndices.x; x <= maxIndices.x; x++)
		for (unsigned y = minIndices.y; y <= maxIndices.y; y++)
			for (unsigned z = minIndices.z; z <= maxIndices.z; z++)
			{
				// potential collision partners
				for (auto & potentialStaticObject : grid->cells[x][y][z].staticObjects)
				{
					potentialCollisionObjects.insert(potentialStaticObject);
				}
				for (auto & potentialObject : grid->cells[x][y][z].objects)
				{
					if (potentialObject != object)
					{
						potentialCollisionObjects.insert(potentialObject);
					}
				}
				grid->insertObject(object, glm::uvec3(x, y, z));
			}

	// check potential collision partners
	for (auto & potentialObject : potentialCollisionObjects)
	{
		CollisionData collision;

		// checking collision of 2 spheres is faster than would be checking collision of their AABBs
		if ((object->model->shape->type != sphere) && (potentialObject->model->shape->type != sphere))
		{
			if (checkCollisionAABBs(object->aabb, potentialObject->aabb))
			{
				// check narrow phase collision and store collision data if collision occured
				if (collisionDetectorNarrow->checkCollision(collision, object, potentialObject))
					collisions.push_back(collision);
			}
		}
		else
		{
			// check narrow phase collision and store collision data if collision occured
			if (collisionDetectorNarrow->checkCollision(collision, object, potentialObject))
				collisions.push_back(collision);
		}
	}

	return true;
}

bool CollisionDetectionBroad::checkCollisionAABBs(AABB* a, AABB* b)
{
	if (a->max.x < b->min.x || a->min.x > b->max.x) 
		return false;
	if (a->max.y < b->min.y || a->min.y > b->max.y) 
		return false;
	if (a->max.z < b->min.z || a->min.z > b->max.z) 
		return false;

	return true;
}

void CollisionDetectionBroad::clearGrid()
{
	grid->clearGrid();
}

bool CollisionDetectionBroad::insertStaticObject(Object* object)
{
	glm::uvec3 minIndices;
	glm::uvec3 maxIndices;

	// indices of min corner of an object's AABB in the grid
	if (!grid->mapPositionToIndices(object->aabb->min, minIndices))
	{
		// can't be inserted into grid
		return false;
	}

	// indices of max corner of an object's AABB in the grid
	if (!grid->mapPositionToIndices(object->aabb->max, maxIndices))
	{
		// can't be inserted into grid
		return false;
	}
	
	// insert object into every cell it occupies
	for (unsigned x = minIndices.x; x <= maxIndices.x; x++)
		for (unsigned y = minIndices.y; y <= maxIndices.y; y++)
			for (unsigned z = minIndices.z; z <= maxIndices.z; z++)
			{
				grid->insertStaticObject(object, glm::uvec3(x, y, z));
			}
	return true;
}
