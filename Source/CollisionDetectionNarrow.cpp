/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	CollisionDetectionNarrow.cpp
 *
 */


#include "CollisionDetectionNarrow.h"


CollisionDetectionNarrow::CollisionDetectionNarrow()
{
}

bool CollisionDetectionNarrow::checkCollision(CollisionData& collision, Object* object0, Object* object1)
{
	ShapeType shapeType0 = object0->model->shape->type;
	ShapeType shapeType1 = object1->model->shape->type;

	bool objectsCollide = false;

	if (shapeType0 == sphere)
	{
		if (shapeType1 == sphere)
		{
			objectsCollide = checkCollisionSpheres(collision, object0, object1);
		}
		else if (shapeType1 == hull)
		{
			objectsCollide = checkCollisionHullSphere(collision, object1, object0);
		}
		else if (shapeType1 == plane)
		{
			objectsCollide = checkCollisionPlaneSphere(collision, object1, object0);
		}
	}
	else if (shapeType0 == plane && shapeType1 == sphere)
	{
		objectsCollide = checkCollisionPlaneSphere(collision, object0, object1);
	}
	else		// shapeType0 == hull || shapeType0 == plane
	{
		if (shapeType1 == sphere)
		{
			if (shapeType0)
			objectsCollide = checkCollisionHullSphere(collision, object0, object1);
		}
		else // shapeType1 == hull || shapeType1 == plane
		{
			objectsCollide = checkCollisionHulls(collision, object0, object1);
		}

	}

	return objectsCollide;
}

bool CollisionDetectionNarrow::checkCollisionHulls(CollisionData& collision, Object * object0, Object * object1)
{
	FaceQuery faceQuery0 = testFaceNormals(object0, object1);
	if (faceQuery0.separationDistance > 0.0f)
	{
		// no collision
		return false;
	}

	FaceQuery faceQuery1 = testFaceNormals(object1, object0);
	if (faceQuery1.separationDistance > 0.0f)
	{
		// no collision
		return false;
	}

	// list of collision points
	std::vector<glm::vec3> collisionPoints;

	// tolerance constants
	constexpr float linearSlop = 0.005f;
	constexpr float relativeFaceTolerance = 0.98f;
	constexpr float absoluteTolerance = 0.5f * linearSlop;

	Object* queryObject0;
	Object* queryObject1;

	glm::vec3 collisionPoint;

	FaceQuery query;
	bool success = false;

	// prefer one face query over other because of coherent contact points from frame to frame
	if (faceQuery1.separationDistance > relativeFaceTolerance * faceQuery0.separationDistance + absoluteTolerance)
	{
		success = createFaceCollision(faceQuery1, collisionPoints);
		query = faceQuery1;
		Hull* hull = dynamic_cast<Hull*>(faceQuery1.object0->model->shape);
		glm::vec3 collisionAxis = faceQuery1.object0->getRotationMatrix() * hull->faces[faceQuery1.faceIndex]->normal;
		query.axis = collisionAxis;
	}
	else
	{
		success = createFaceCollision(faceQuery0, collisionPoints);
		query = faceQuery0;
		Hull* hull = dynamic_cast<Hull*>(faceQuery0.object0->model->shape);
		glm::vec3 collisionAxis = faceQuery0.object0->getRotationMatrix() * hull->faces[faceQuery0.faceIndex]->normal;
		query.axis = collisionAxis;
	}
	if (!success)
		return false;

	glm::vec3 sum = glm::vec3(0.0f);
	unsigned collisionPointsCount = collisionPoints.size();

	for (unsigned i = 0; i < collisionPointsCount; i++)
	{
		sum += collisionPoints[i];
	}
	collisionPoint = sum / (float)collisionPointsCount;

	// couldn't create collision
	if (collisionPoints.empty())
		return false;

	glm::vec3 collisionNormal = glm::normalize(query.axis);

	queryObject0 = query.object0;
	queryObject1 = query.object1;

	float seperationDistance = std::abs(query.separationDistance);

	auto position0 = queryObject0->getPosition();
	auto position1 = queryObject1->getPosition();

	if (glm::dot(position0 - position1, collisionNormal) < 0.0)
	{
		// collision normal doesn't point toward first object, flip it
		collisionNormal = -collisionNormal;
	}

	pushObjectsOutOfCollision(queryObject0, queryObject1, collisionNormal, seperationDistance);

	collision.object0 = queryObject0;
	collision.object1 = queryObject1;
	collision.collisionNormal = collisionNormal;
	collision.collisionPoint = collisionPoint;

	return true;
}

bool CollisionDetectionNarrow::checkCollisionSpheres(CollisionData& collision, Object* object0, Object* object1)
{
	Sphere* sphere0 = dynamic_cast<Sphere*>(object0->model->shape);
	Sphere* sphere1 = dynamic_cast<Sphere*>(object1->model->shape);

	glm::vec3 position0 = object0->configuration.position;
	glm::vec3 position1 = object1->configuration.position;

	float radius0 = sphere0->radius;
	float radius1 = sphere1->radius;

	float distance = glm::length(position1 - position0);
	float sumOfRadii = radius0 + radius1;

	float separation = distance - sumOfRadii;

	if (separation > 0.0f)
	{
		// no separation
		return false;
	}

	glm::vec3 collisionNormal;
	if (position0 == position1)
	{
		// spheres are coincident
		collisionNormal = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else
	{
		collisionNormal = glm::normalize(position0 - position1);
	}

	separation = std::abs(separation);
	
	pushObjectsOutOfCollision(object0, object1, collisionNormal, separation);

	glm::vec3 collisionPoint = object1->getPosition() + collisionNormal * sphere1->radius;

	collision.object0 = object0;
	collision.object1 = object1;
	collision.collisionNormal = collisionNormal;
	collision.collisionPoint = collisionPoint;

	return true;
}

bool CollisionDetectionNarrow::checkCollisionPlaneSphere(CollisionData& collision, Object* planeObject, Object* sphereObject)
{
	PlaneShape* plane = dynamic_cast<PlaneShape*>(planeObject->model->shape);
	Sphere* sphere = dynamic_cast<Sphere*>(sphereObject->model->shape);

	glm::mat4 transformationMatrix = planeObject->getModelMatrix();
	glm::mat3 rotationMatrix = planeObject->getRotationMatrix();

	HalfEdge* firstEdge = *(plane->edges.begin());
	HalfEdge* edge = firstEdge;

	// check if sphere is out of plane
	do
	{
		heVertex sidePlaneVertex = { glm::vec3(transformationMatrix * glm::vec4(edge->head->position, 1.0f)) };

		// normal of this side plane is direction vector of previous edge
		glm::vec3 sidePlaneNormal = rotationMatrix * edge->previous->directionVector;

		Plane sidePlane = { &sidePlaneVertex, sidePlaneNormal };

		heVertex supportPoint = { sphere->getSupport(-sidePlane.normal, sphereObject->getPosition()) };
		float distance = supportPoint.distanceFromPlane(sidePlane);

		if (distance > 0.0f)
		{
			return false;
		}

		edge = edge->next;
	} while (edge != firstEdge);

	return checkCollisionHullSphere(collision, planeObject, sphereObject);
}


bool CollisionDetectionNarrow::checkCollisionHullSphere(CollisionData& collision, Object* hullObject, Object* sphereObject)
{
	Hull* hull = dynamic_cast<Hull*>(hullObject->model->shape);
	Sphere* sphere = dynamic_cast<Sphere*>(sphereObject->model->shape);

	glm::mat4 transformationMatrix = hullObject->getModelMatrix();
	glm::mat3 rotationMatrix = hullObject->getRotationMatrix();
	glm::vec3 spherePosition = sphereObject->getPosition();

	glm::vec3 collisionNormal = glm::vec3(0.0f);

	float bestDistance = -FLT_MAX;
	int bestFaceIndex = -1;

	for (unsigned int i = 0; i < hull->faces.size(); i++)
	{
		auto face = hull->faces[i];
		heVertex pointOnPlane = { transformationMatrix * glm::vec4(face->edge->tail->position, 1.0f) };
		glm::vec3 planeNormal = rotationMatrix * face->normal;

		Plane planeA = { &pointOnPlane, planeNormal };
		heVertex supportPoint = { sphere->getSupport(-planeA.normal, spherePosition) };

		float distance = supportPoint.distanceFromPlane(planeA);

		if (distance > 0.0f)
		{
			return false;
		}

		if (distance > bestDistance)
		{
			bestDistance = distance;
			bestFaceIndex = i;
			collisionNormal = planeNormal;
		}
	}

	if (bestFaceIndex < 0)
		return false;

	bestDistance = std::abs(bestDistance);

	for (auto & edge : hull->uniqueEdges)
	{
		glm::vec3 edgeCenter = (edge->tail->position + edge->head->position) / 2.0f;
		edgeCenter = transformationMatrix * glm::vec4(edgeCenter, 1.0f);
		glm::vec3 axis = rotationMatrix * glm::normalize(edgeCenter - spherePosition);

		interval hullProjectionInterval = hull->getProjectionInterval(axis, transformationMatrix);
		interval sphereProjectionInterval = sphere->getProjectionInterval(axis, spherePosition);

		if (!overlap(hullProjectionInterval, sphereProjectionInterval))
		{
			return false;
		}

		float overlapAmount;

		if (hullProjectionInterval.min < sphereProjectionInterval.min)
		{
			overlapAmount = std::abs(hullProjectionInterval.max - sphereProjectionInterval.min);
		}
		else
		{
			overlapAmount = std::abs(sphereProjectionInterval.max - hullProjectionInterval.min);
		}

		if (overlapAmount < bestDistance)
		{
			bestDistance = overlapAmount;
			collisionNormal = axis;
		}
	}

	if (glm::dot(hullObject->getPosition() - sphereObject->getPosition(), collisionNormal) < 0.0)
	{
		// collision normal doesn't point toward hull object, flip it
		collisionNormal = -collisionNormal;
	}

	pushObjectsOutOfCollision(hullObject, sphereObject, collisionNormal, bestDistance);

	glm::vec3 collisionPoint = sphereObject->getPosition() + collisionNormal * sphere->radius;

	collision.object0 = hullObject;
	collision.object1 = sphereObject;
	collision.collisionNormal = collisionNormal;
	collision.collisionPoint = collisionPoint;

	return true;
}

void CollisionDetectionNarrow::pushObjectsOutOfCollision(Object* object0, Object* object1, const glm::vec3& collisionNormal, float seperationDistance)
{
	Object::Configuration& configuration0 = object0->configuration;
	Object::Configuration& configuration1 = object1->configuration;

	// push objects out of collision
	glm::vec3 push = collisionNormal * seperationDistance;

	if (object0->mass != INFINITY && object1->mass != INFINITY)
		// both objects are dynamic
		push = push / 2.0f;

	if (object0->density != INFINITY)
		configuration0.position += push;

	if (object1->density != INFINITY)
		configuration1.position -= push;
}

void CollisionDetectionNarrow::SutherlandHodgman(const std::vector<heVertex>& polygon, const Plane& plane, std::vector<heVertex>& out)
{
	// tail vertex of an edge
	heVertex vertex1 = polygon.back();
	float distance1 = vertex1.distanceFromPlane(plane);

	for (unsigned int i = 0; i < polygon.size(); i++)
	{
		// head vertex of an edge
		heVertex vertex2 = polygon[i];
		float distance2 = vertex2.distanceFromPlane(plane);

		if (distance1 <= 0.0f && distance2 <= 0.0f)
		{
			// both vertices are behind the plane
			out.push_back(vertex2);
		}
		else if (distance1 <= 0.0f && distance2 > 0.0f)
		{
			// vertex1 is behind the plane, vertex 2 in front -> intersection
			float fraction = distance1 / (distance1 - distance2);
			glm::vec3 intersection = vertex1.position + fraction * (vertex2.position - vertex1.position);

			// keep intersection point
			out.push_back(intersection);
		}
		else if (distance2 <= 0.0f && distance1 > 0.0f)
		{
			// vertex2 is behind the plane, vertex 1 is in front
			float fraction = distance1 / (distance1 - distance2);
			glm::vec3 intersection = vertex1.position + fraction * (vertex2.position - vertex1.position);

			// keep intersection point
			out.push_back(intersection);

			// and also vertex2
			out.push_back(vertex2);
		}
		// keep vertex 2 as tail of the next edge
		vertex1 = vertex2;
		distance1 = distance2;
	}
}

bool CollisionDetectionNarrow::createFaceCollision(FaceQuery& collisionQuery, std::vector<glm::vec3>& out)
{
	Object* object0 = collisionQuery.object0;
	Object* object1 = collisionQuery.object1;

	glm::mat3 rotationMatrixObject0 = object0->getRotationMatrix();
	glm::mat4 transformationMatrixObject0 = object0->getModelMatrix();
	glm::mat4 transformationMatrixObject1 = object1->getModelMatrix();

	Hull* hull = dynamic_cast<Hull*>(object0->model->shape);

	heFace* referenceFace = hull->faces[collisionQuery.faceIndex];
	heFace* incidentFace = collisionQuery.findIncidentFace();

	heVertex referenceFaceVertex = { transformationMatrixObject0 * glm::vec4(referenceFace->edge->tail->position, 1.0f) };
	glm::vec3 referenceFaceNormal = rotationMatrixObject0 * referenceFace->normal;
	Plane referencePlane = { &referenceFaceVertex, referenceFaceNormal };

	HalfEdge* firstEdge = incidentFace->edge;
	HalfEdge* currentEdge = firstEdge;
	std::vector<heVertex> polygon;

	// iterate through all incident face edges and build polygon
	do
	{
		heVertex vertex = { currentEdge->tail->position };
		vertex.position = transformationMatrixObject1 * glm::vec4(vertex.position, 1.0f);

		polygon.push_back(vertex);
		currentEdge = currentEdge->next;
	} while (currentEdge != firstEdge);

	firstEdge = referenceFace->edge;
	currentEdge = firstEdge;

	// iterate through all reference face edges
	do
	{
		heVertex vertexA = { glm::vec3(transformationMatrixObject0 * glm::vec4(currentEdge->tail->position, 1.0f)) };
		heVertex vertexB = { glm::vec3(transformationMatrixObject0 * glm::vec4(currentEdge->head->position, 1.0f)) };

		// create side plane of an edge
		glm::vec3 planeNormal = glm::cross(vertexB.position - vertexA.position, rotationMatrixObject0 * referenceFace->normal);
		planeNormal = glm::normalize(planeNormal);
		Plane clippingPlane = { &vertexA, planeNormal };

		std::vector<heVertex> newPolygon = std::vector<heVertex>();

		// clip plane with polygon (incident face)
		SutherlandHodgman(polygon, clippingPlane, newPolygon);

		polygon = newPolygon;

		if (polygon.size() < 3)
			return false;

		currentEdge = currentEdge->next;
	} while (currentEdge != firstEdge);


	for (auto & vertex : polygon)
	{
		float distance;
		// keep only vertices that are below reference face
		if ((distance = vertex.distanceFromPlane(referencePlane)) < 0.0f)
		{
			// move contact point onto referencePlane
			glm::vec3 contactPoint = vertex.position + referencePlane.normal * std::abs(distance);
			out.push_back(contactPoint);
		}
	}
	return true;
}


CollisionDetectionNarrow::Query::Query()
{
	object0 = NULL;
	object1 = NULL;
	axis = glm::vec3(0.0f);
	separationDistance = 0.0f;
}

heFace* CollisionDetectionNarrow::FaceQuery::findIncidentFace()
{
	Hull* hull0 = dynamic_cast<Hull*>(object0->model->shape);
	Hull* hull1 = dynamic_cast<Hull*>(object1->model->shape);

	heFace* referenceFace = hull0->faces[faceIndex];
	glm::vec3 referenceFaceNormal = object0->getRotationMatrix() * referenceFace->normal;

	glm::mat3 rotationMatrixObject1 = object1->getRotationMatrix();

	heFace* incidentFace = hull1->faces[0];
	glm::vec3 faceNormal = rotationMatrixObject1 * incidentFace->normal;
	float minimalDot = glm::dot(faceNormal, referenceFaceNormal);
	

	for (unsigned int i = 1; i < hull1->faces.size(); i++)
	{
		heFace* face = hull1->faces[i];
		faceNormal = rotationMatrixObject1 * face->normal;
		float dot = glm::dot(faceNormal, referenceFaceNormal);

		// incident face is face with smallest dot product with reference face normal
		if (dot < minimalDot)
		{
			minimalDot = dot;
			incidentFace = face;
		}
	}

	return incidentFace;
}


bool CollisionDetectionNarrow::overlap(interval interval0, interval interval1)
{
	return !((interval0.max < interval1.min) || (interval1.max < interval0.min));
}


CollisionDetectionNarrow::FaceQuery CollisionDetectionNarrow::testFaceNormals(Object* object0, Object* object1)
{
	FaceQuery query;
	query.object0 = object0;
	query.object1 = object1;
	query.faceIndex = -1;
	query.separationDistance = FLT_MAX;

	Hull* hull0 = dynamic_cast<Hull*>(object0->model->shape);
	Hull* hull1 = dynamic_cast<Hull*>(object1->model->shape);

	float bestDistance = -FLT_MAX;
	unsigned int bestFaceIndex = -1;
	unsigned int faceCount = hull0->faces.size();
	const auto & faces = hull0->faces;

	glm::mat4 transformationMatrixObject0 = object0->getModelMatrix();
	glm::mat4 transformationMatrixObject1 = object1->getModelMatrix();

	glm::mat3 rotationMatrix = object0->getRotationMatrix();

	for(unsigned int i = 0; i < faceCount; i++)
	{
		auto face = faces[i];
		heVertex pointOnPlane = { transformationMatrixObject0 * glm::vec4(face->edge->tail->position, 1.0f) };
		glm::vec3 planeNormal = rotationMatrix * face->normal;

		Plane planeA = { &pointOnPlane, planeNormal };
		heVertex supportPoint = { hull1->getSupport(-planeA.normal, transformationMatrixObject1) };

		float distance = supportPoint.distanceFromPlane(planeA);

		if (distance > 0.0f)
		{
			query.separationDistance = FLT_MAX;
			return query;
		}

		if (distance > bestDistance)
		{
			bestDistance = distance;
			bestFaceIndex = i;
		}
	}

	query.faceIndex = bestFaceIndex;
	query.separationDistance = bestDistance;

	return query;
}

bool isZeroVector(glm::vec3 vector)
{
	return std::abs(glm::length(vector)) < EPSILON;
}
