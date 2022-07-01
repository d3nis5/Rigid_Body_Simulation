/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Grid.h
 *
 */

#include "Grid.h"
#include <glm/gtx/string_cast.hpp>
#include <glm/common.hpp>


Cell::Cell() = default;

void Cell::clear()
{
	objects.clear();
}

Grid::Grid()
{
	try
	{
		cells = new Cell**[GRID_HEIGHT];
		for (int i = 0; i < GRID_HEIGHT; i++)
		{
			cells[i] = new Cell*[GRID_WIDTH];
			for (int j = 0; j < GRID_WIDTH; j++)
				cells[i][j] = new Cell[GRID_DEPTH];
		}
	}
	catch (std::bad_alloc)
	{
		std::cout << "Couldn't allocate memory for grid" << std::endl;
		cells = NULL;
	}
}


Grid::~Grid()
{
	for (int i = 0; i < GRID_HEIGHT; i++)
	{
		for (int j = 0; j < GRID_DEPTH; j++)
			delete[] cells[i][j];
		delete[] cells[i];
	}
	delete[] cells;
}

void Grid::insertObject(Object* object, const glm::uvec3& indices)
{
	Cell* cell = &(cells[indices.x][indices.y][indices.z]);
	cell->objects.insert(object);
	occupiedCells.insert(cell);
}

void Grid::insertStaticObject(Object* object, const glm::uvec3& indices)
{
	Cell* cell = &(cells[indices.x][indices.y][indices.z]);
	cell->staticObjects.push_back(object);
}

void Grid::clearGrid()
{
	for (auto & cell : occupiedCells)
	{
		cell->clear();
	}
	occupiedCells.clear();
}

bool Grid::mapPositionToIndices(const glm::vec3& position, glm::uvec3& indices)
{
	// height index
	if ((MIN_HEIGHT_COORD - CELL_SIZE > position.y) || (MAX_HEIGHT_COORD + CELL_SIZE < position.y))
	{
		// object is out of grid
		return false;
	}
	else if ((MIN_HEIGHT_COORD > position.y) && (MIN_HEIGHT_COORD - CELL_SIZE < position.y))
	{
		// position is on the edge of being in the grid or out of grid
		indices.x = 0;
	}
	else
	{
		indices.x = static_cast<unsigned int>((position.y - MIN_HEIGHT_COORD) / CELL_SIZE);

		if (indices.x >= MAX_HEIGHT)
		{
			indices.x = MAX_HEIGHT;
		}
	}

	// width index
	if ((MIN_WIDTH_COORD - CELL_SIZE > position.x) || (MAX_WIDTH_COORD + CELL_SIZE < position.x))
	{
		// object is out of grid
		return false;
	}
	else
	{
		indices.y = static_cast<unsigned int>((position.x - MIN_WIDTH_COORD) / CELL_SIZE);

		if (indices.y >= MAX_WIDTH)
		{
			indices.y = MAX_WIDTH;
		}
	}
	// depth index
	if ((MIN_DEPTH_COORD - CELL_SIZE > position.z) || (MAX_DEPTH_COORD + CELL_SIZE < position.z))
	{
		// object is out of grid
		return false;
	}
	else
	{
		indices.z = static_cast<unsigned int>((position.z - MIN_DEPTH_COORD) / CELL_SIZE);

		if (indices.z >= MAX_DEPTH)
		{
			indices.z = MAX_DEPTH;
		}
	}

	return true;
}


