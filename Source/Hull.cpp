/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Hull.cpp
 *
 */

#include "Hull.h"

Hull::Hull()
{
	type = hull;
}


Hull::~Hull()
{
	unsigned int i;

	// delete faces
	for (i = 0; i < faces.size(); i++)
		delete faces[i];

	// delete edges
	for (auto iterator = edges.begin(); iterator != edges.end(); iterator++)
		delete *iterator;

	// delete vertices
	for (i = 0; i < vertices.size(); i++)
		delete vertices[i];
}

bool Hull::addVertex(glm::vec3 position)
{
	heVertex* newVertex;

	try
	{
		newVertex = new heVertex(position);
	}
	catch (...)
	{
		return false;
	}
	this->vertices.push_back(newVertex);

	return true;
}

bool Hull::addFace(int vertex1, int vertex2, int vertex3, glm::vec3 normal)
{
	HalfEdge* newEdge1 = createEdges(vertex1, vertex2, vertex3);

	if (newEdge1 == NULL)
	{
		return false;
	}

	HalfEdge* newEdge2 = newEdge1->next;
	HalfEdge* newEdge3 = newEdge2->next;

	heFace* newFace;

	try
	{
		newFace = new heFace(newEdge1, normal);
	}
	catch (...)
	{
		return false;
	}

	faces.push_back(newFace);

	newEdge1->face = newFace;
	newEdge2->face = newFace;
	newEdge3->face = newFace;

	if (!mergeFaceIfCoplanar(newEdge1))
	{
		edges.insert(newEdge1);
	}

	if (!mergeFaceIfCoplanar(newEdge2))
	{
		edges.insert(newEdge2);
	}

	if (!mergeFaceIfCoplanar(newEdge3))
	{
		edges.insert(newEdge3);
	}

	return true;
}

void Hull::finalizeBuild()
{
	for (auto & vertexPair : vertexPairs)
	{
		uniqueEdges.push_back(vertexPair.second);
	}

	vertexPairs.clear();
}

interval Hull::getProjectionInterval(glm::vec3 axis, const glm::mat4& transformationMatrix)
{
	interval result;

	glm::vec3 vertex0 = transformationMatrix * glm::vec4(this->vertices[0]->position, 1.0f);

	result.min = glm::dot(vertex0, axis);
	result.max = result.min;

	// iterate through all vertices of hull and project them onto axis
	for (unsigned int i = 1; i < this->vertices.size(); i++)
	{
		glm::vec3 point = transformationMatrix * glm::vec4(this->vertices[i]->position, 1.0);
		float projectedVertex = glm::dot(point, axis);

		if (projectedVertex < result.min)
		{
			result.min = projectedVertex;
		}
		else if (projectedVertex > result.max)
		{
			result.max = projectedVertex;
		}
	}
	return result;
}

glm::vec3 Hull::getSupport(const glm::vec3& direction, const glm::mat4& transformationMatrix)
{
	heVertex* vertex;
	glm::vec3 bestVertex = glm::vec3(FLT_MAX);

	float projection;
	float bestProjection = -FLT_MAX;

	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		vertex = vertices[i];
		glm::vec3 vertexPosition = transformationMatrix * glm::vec4(vertex->position, 1.0f);
		projection = glm::dot(vertexPosition, direction);

		if (projection > bestProjection)
		{
			bestProjection = projection;
			bestVertex = vertexPosition;
		}
	}
	return bestVertex;
}

/**
 * Algorithm from https://www.geometrictools.com/Documentation/PolyhedralMassProperties.pdf
 * Author: David Eberly
 */
void Hull::calculateAttributes(Object* object)
{
	const float mult[10] = { 1.0f / 6.0f, 1.0f / 24.0f, 1.0f / 24.0f, 1.0f / 24.0f,
		1.0f / 60.0f, 1.0f / 60.0f, 1.0f / 60.0f, 1.0f / 120.0f, 1.0f / 120.0f, 1.0f / 120.0f };

	float intg[10] = { 0.0f };
	const auto & vertexBuffer = object->model->vertexBuffer;
	const auto & indexBuffer = object->model->indexBuffer;
	unsigned numberOfTraingles = indexBuffer.size() / 3;

	for (unsigned t = 0; t < numberOfTraingles; t++)
	{
		// get vertices of triangle
		unsigned i0 = indexBuffer[3 * t];
		unsigned i1 = indexBuffer[3 * t + 1];
		unsigned i2 = indexBuffer[3 * t + 2];
		float x0, y0, z0, x1, y1, z1, x2, y2, z2;

		glm::vec3 vertex0 = vertexBuffer[i0].position;
		glm::vec3 vertex1 = vertexBuffer[i1].position;
		glm::vec3 vertex2 = vertexBuffer[i2].position;

		x0 = vertex0.x; y0 = vertex0.y, z0 = vertex0.z;
		x1 = vertex1.x; y1 = vertex1.y, z1 = vertex1.z;
		x2 = vertex2.x; y2 = vertex2.y, z2 = vertex2.z;

		// get edges and cross products of edges
		float a1, b1, c1, a2, b2, c2, d0, d1, d2;
		a1 = x1 - x0; b1 = y1 - y0; c1 = z1 - z0; a2 = x2 - x0; b2 = y2 - y0; c2 = z2 - z0;
		d0 = b1 * c2 - b2 * c1; d1 = a2 * c1 - a1 * c2; d2 = a1 * b2 - a2 * b1;

		// recompute integral terms
		float f1x, f1y, f1z, f2x, f2y, f2z, f3x, f3y, f3z;
		float g0x, g0y, g0z, g1x, g1y, g1z, g2x, g2y, g2z;
		subexpressions(x0, x1, x2, f1x, f2x, f3x, g0x, g1x, g2x);
		subexpressions(y0, y1, y2, f1y, f2y, f3y, g0y, g1y, g2y);
		subexpressions(z0, z1, z2, f1z, f2z, f3z, g0z, g1z, g2z);

		// update integrals
		intg[0] += d0 * f1x;
		intg[1] += d0 * f2x; intg[2] += d1 * f2y; intg[3] += d2 * f2z;
		intg[4] += d0 * f3x; intg[5] += d1 * f3y; intg[6] += d2 * f3z;
		intg[7] += d0 * (y0*g0x + y1 * g1x + y2 * g2x);
		intg[8] += d1 * (z0*g0y + z1 * g1y + z2 * g2y);
		intg[9] += d2 * (x0*g0z + x1 * g1z + 2 * g2z);
	}

	for (int i = 0; i < 10; i++)
		intg[i] *= mult[i];

	float mass = intg[0];

	// center of mass
	glm::vec3 cm;
	cm.x = intg[1] / mass;
	cm.y = intg[2] / mass;
	cm.z = intg[3] / mass;

	// inertia tensor relative to center of mass
	glm::mat3 inertia;

	inertia[0][0] = intg[5] + intg[6] - mass * (cm.y*cm.y + cm.z*cm.z);		// xx
	inertia[1][1] = intg[4] + intg[6] - mass * (cm.z*cm.z + cm.x*cm.x);		// yy
	inertia[2][2] = intg[4] + intg[5] - mass * (cm.x*cm.x + cm.y*cm.y);		// zz
	inertia[0][1] = -(intg[7] - mass * cm.x*cm.y);		// xy
	inertia[1][2] = -(intg[8] - mass * cm.y*cm.z);		// yz
	inertia[0][2] = -(intg[9] - mass * cm.z*cm.x);		// xz
	inertia[1][0] = inertia[0][1];	// yx
	inertia[2][1] = inertia[1][2];	// zy
	inertia[2][0] = inertia[0][2];	// zx

	object->centerOfMass = cm;
	object->mass = mass * object->density;
	object->inverseMass = 1.0f / object->mass;
	object->inverseBodyInertiaTensor = glm::inverse(inertia * object->density);
}

void Hull::subexpressions(float w0, float w1, float w2, float& f1, float& f2, float& f3, float& g0, float& g1, float& g2)
{
	float temp0 = w0 + w1;
	float temp1 = w0 * w0;
	float temp2 = temp1 + w1 * temp0;
	f1 = temp0 + w2; f2 = temp2 + w2 * f1; f3 = w0 * temp1 + w1 * temp2 + w2 * f2;
	g0 = f2 + w0 * (f1 + w0); g1 = f2 + w1 * (f1 + w1); g2 = f2 + w2 * (f1 + w2);
}

HalfEdge* Hull::createEdges(int& vertex1, int& vertex2, int& vertex3)
{
	HalfEdge* newEdge1;
	HalfEdge* newEdge2;
	HalfEdge* newEdge3;

	try
	{
		newEdge1 = new HalfEdge;
		newEdge2 = new HalfEdge;
		newEdge3 = new HalfEdge;
	}
	catch (...)
	{
		return NULL;
	}

	newEdge1->tail = vertices[vertex1];
	newEdge1->head = vertices[vertex2];
	newEdge1->calculateDirectionVector();
	checkForTwinEdge(newEdge1);

	newEdge2->tail = vertices[vertex2];
	newEdge2->head = vertices[vertex3];
	newEdge2->calculateDirectionVector();
	checkForTwinEdge(newEdge2);

	newEdge3->tail = vertices[vertex3];
	newEdge3->head = vertices[vertex1];
	newEdge3->calculateDirectionVector();
	checkForTwinEdge(newEdge3);

	newEdge1->next = newEdge2;
	newEdge1->previous = newEdge3;

	newEdge2->next = newEdge3;
	newEdge2->previous = newEdge1;

	newEdge3->next = newEdge1;
	newEdge3->previous = newEdge2;

	return newEdge1;
}

void Hull::checkForTwinEdge(HalfEdge* edge)
{
	auto twin = findTwin(edge);

	if (twin)
	{
		edge->twin = twin;
		twin->twin = edge;
	}
	else
	{
		vertexPairs.insert({ std::make_pair(edge->tail, edge->head), edge });
	}
}


bool Hull::mergeFaceIfCoplanar(HalfEdge* edge)
{
	auto twin = edge->twin;
	if (twin != NULL)
	{
		if (edge->face->isCoplanar(twin->face))
		{
			mergeFaces(edge);
			return true;
		}
	}
	return false;
}


size_t Hull::vertexPairHash::operator()(const std::pair<heVertex*, heVertex*>& pair) const
{
	return std::hash<heVertex*>() (pair.first) ^ std::hash<heVertex*>() (pair.second);
}


HalfEdge* Hull::findTwin(HalfEdge* edge)
{
	auto twinIterator = vertexPairs.find({edge->head, edge->tail});

	if (twinIterator == vertexPairs.end())
	{
		return NULL;
	}

	auto twinEdge = twinIterator->second;

	return twinEdge;
}

void Hull::mergeFaces(HalfEdge* edge)
{
	auto twin = edge->twin;
	auto faceToDelete =  edge->face;

	// set edge reference
	twin->face->edge = twin->previous;

	// absorb face
	auto ptr = edge->next;
	do
	{
		ptr->face = twin->face;
		ptr = ptr->next;
	} while (ptr != edge->next);

	// link edges
	twin->previous->next = edge->next;
	twin->next->previous = edge->previous;
	edge->previous->next = twin->next;
	edge->next->previous = twin->previous;

	for (auto it = faces.begin(); it != faces.end(); it++)
	{
		if (*it == faceToDelete)
		{
			faces.erase(it);
			break;
		}
	}

	delete faceToDelete;

	vertexPairs.erase({edge->tail, edge->head});
	vertexPairs.erase({ edge->head, edge->tail });

	edges.erase(twin);
	delete edge;
	delete twin;
}

