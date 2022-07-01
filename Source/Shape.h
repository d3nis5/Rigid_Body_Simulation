/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Shape.cpp
 *
 */

#pragma once

#ifndef SHAPE_H
#define SHAPE_H

class Object;		// forward declaration

enum ShapeType {sphere, hull, plane, none};

 /**
  * @brief Abstract class for shapes
  */
class Shape
{
public:
	ShapeType type;
	Shape();
	virtual ~Shape();

	// calculates attributes (mass, center of mass, inertia tensor) for given object
	virtual void calculateAttributes(Object* object) = 0;
};

#endif