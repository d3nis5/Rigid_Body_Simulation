/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Model.cpp
 *
 */

#include "Object.h"
#include <glm/gtx/string_cast.hpp>

Object::Configuration::Configuration()
{
	position = glm::vec3(0.0f);
	rotation = glm::mat3(0.0f);
	velocityAccumulator = glm::vec3(0.0f);
	velocityVector = glm::vec3(0.0f);
	angularMomentum = glm::vec3(0.0f);
	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);
	inverseWorldInertiaTensor = glm::mat3(0.0f);
	angularVelocity = glm::vec3(0.0f);
}

Object::ObjectInit::ObjectInit()
{
	std::string objectName = "";
	Model* model = NULL;
	glm::vec3 color = glm::vec3(0.0f);
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 initialVelocity = glm::vec3(0.0f);
	float density = 0.0f;
}

Object::Object(ObjectInit initValues)
{
	objectName = initValues.objectName;
	model = initValues.model;
	color = initValues.color;
	configuration.position = initValues.position;
	configuration.rotation = constructRotationMatrix(initValues.rotation);
	configuration.velocityVector = initValues.initialVelocity;
	density = initValues.density;
	aabb = new AABB(this);

	if (density == INFINITY)
	{
		mass = INFINITY;
		inverseMass = 0.0f;
		centerOfMass = glm::vec3(0.0f);
		inverseBodyInertiaTensor = glm::mat3(0.0f);
	}
	else
	{
		model->shape->calculateAttributes(this);
	}
}

Object::~Object()
{
	if (aabb != NULL)
		delete aabb;
}

glm::mat4 Object::getModelMatrix()
{
	glm::mat4 transformationMatrix = glm::mat4(1.0f);

	transformationMatrix = glm::translate(transformationMatrix, configuration.position);
	
	return transformationMatrix * glm::mat4(configuration.rotation);
}

glm::mat3 Object::getRotationMatrix()
{
	return configuration.rotation;
}

glm::vec3 Object::getPosition()
{
	return configuration.position;
}

void Object::computeInverseWorldInertiaTensor()
{
	configuration.inverseWorldInertiaTensor = configuration.rotation * inverseBodyInertiaTensor * glm::transpose(configuration.rotation);
}

glm::mat3 Object::constructRotationMatrix(glm::vec3 eulerAngles)
{
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationMatrix = glm::rotate(rotationMatrix, glm::radians(eulerAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));

	return rotationMatrix;
}


void Object::reorthogonalizeRotationMatrix()
{
	glm::mat3& rotation = configuration.rotation;

	glm::vec3 x = glm::vec3(rotation[0][0], rotation[1][0], rotation[2][0]);
	glm::vec3 y = glm::vec3(rotation[0][1], rotation[1][1], rotation[2][1]);
	glm::vec3 z;

	x = glm::normalize(x);
	z = glm::normalize(glm::cross(x, y));
	y = glm::normalize(glm::cross(z, x));

	rotation[0][0] = x.x; rotation[0][1] = y.x; rotation[0][2] = z.x;
	rotation[1][0] = x.y; rotation[1][1] = y.y; rotation[1][2] = z.y;
	rotation[2][0] = x.z; rotation[2][1] = y.z; rotation[2][2] = z.z;
}

AABB::AABB(Object* object)
{
	recomputeAABB(object);
}

void AABB::recomputeAABB(Object* object)
{
	Shape* shape = object->model->shape;

	if (shape->type == sphere)
	{
		// object is sphere
		Sphere* sphere = dynamic_cast<Sphere*>(shape);
		float radius = sphere->radius;
		glm::vec3 center = object->getPosition();

		this->min = glm::vec3(center - radius);
		this->max = glm::vec3(center + radius);
	}
	else
	{
		glm::mat4 transformMatrix = object->getModelMatrix();

		Hull* hull = dynamic_cast<Hull*>(shape);

		const auto & vertices = hull->vertices;

		glm::vec3 minCoords = transformMatrix * glm::vec4(vertices[0]->position, 1.0f);
		glm::vec3 maxCoords = transformMatrix * glm::vec4(vertices[0]->position, 1.0f);

		for (unsigned i = 1; i < hull->vertices.size(); i++)
		{
			glm::vec3 vertex = transformMatrix * glm::vec4(vertices[i]->position, 1.0f);

			assignMinMax(vertex.x, minCoords.x, maxCoords.x);
			assignMinMax(vertex.y, minCoords.y, maxCoords.y);
			assignMinMax(vertex.z, minCoords.z, maxCoords.z);
		}
		this->min = minCoords;
		this->max = maxCoords;
	}
}

void AABB::assignMinMax(float value, float& min, float& max)
{
	if (value > max)
	{
		max = value;
	}
	else if (value < min)
	{
		min = value;
	}
}
