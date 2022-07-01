/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Model.cpp
 *
 */

#include "Model.h"

Model::Model(std::string name)
{
	modelName = name;
	shape = NULL;
}

Model::~Model()
{
	if (shape != NULL)
		delete shape;
}
