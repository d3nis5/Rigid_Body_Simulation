/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Grid.h
 *
 */

#pragma once

#include <vector>
#include "Object.h"

#ifndef GRID_H
#define GRID_H

#define CELL_SIZE	3
#define GRID_HEIGHT 50
#define GRID_WIDTH  70		// must be even number
#define GRID_DEPTH  70		// must be even number

#define MAX_HEIGHT (GRID_HEIGHT - 1)
#define MAX_WIDTH  (GRID_WIDTH - 1)
#define MAX_DEPTH  (GRID_DEPTH - 1)

#define MIN_HEIGHT_COORD 0
#define MAX_HEIGHT_COORD (GRID_HEIGHT * CELL_SIZE)
#define MIN_WIDTH_COORD (-(GRID_WIDTH / 2) * CELL_SIZE)
#define MAX_WIDTH_COORD ((GRID_WIDTH / 2) * CELL_SIZE)
#define MIN_DEPTH_COORD (-(GRID_DEPTH / 2) * CELL_SIZE)
#define MAX_DEPTH_COORD ((GRID_DEPTH / 2) * CELL_SIZE)

/**
 * @brief Cell of a grid
 */
class Cell
{
public:
	std::unordered_set<Object*> objects;
	std::vector<Object*> staticObjects;

	Cell();

	/**
	 * @brief Removes all dynamic objects from cell
	 */
	void clear();
};


/**
 * @brief Class representing 3D uniform grid for broad phase collision detection
 */
class Grid
{
public:
	// 3-dimensional array of cells
	// access: cells[height][width][depth]
	Cell*** cells;

	// holds occupied cells
	std::unordered_set<Cell*> occupiedCells;

	/**
	 * @brief Creates 3-dimensional array of cells, or sets cells to NULL if memory could not be allocated
	 */
	Grid();
	~Grid();

	/**
	 * @brief Inserts object into grid
	 * @param object Object to be inserted
	 * @param indices Indices of cell into which to insert object
	 */
	void insertObject(Object* object, const glm::uvec3& indices);

	/**
	 * @brief Inserts  static object into grid
	 * @param object Object to be inserted
	 * @param indices Indices of cell into which to insert object
	 */
	void insertStaticObject(Object* object, const glm::uvec3& indices);

	/**
	 * @brief Clears occupied cells
	 */
	void clearGrid();

	/**
	 * @brief Calculates indices to cells array based on a position
	 * @param position Position to be mapped to indices
	 * @param[out] indices Mapped indices in format x: height index, y: width index, z: depth index
	 * @return Whether position can be mapped to cells array indices
	 */
	bool mapPositionToIndices(const glm::vec3& position, glm::uvec3& indices);
};

#endif