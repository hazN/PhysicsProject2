#include "PhysicsSystem.h"
#include "PhysicsUtils.h"
#include <DirectXMath.h>
#include <iostream>
#include <glm/geometric.hpp>

#include "cMeshObject.h"
#include "globalThings.h"

PhysicsSystem::PhysicsSystem() {
}

PhysicsSystem::~PhysicsSystem() {
	for (int i = 0; i < m_PhysicsObjects.size(); i++) {
		delete m_PhysicsObjects[i];
	}
	m_PhysicsObjects.clear();

	// delete[] m_Particles[0];		Can check if this works (Not sure)
	//  -> edit: Nope, use this for array of pointers
}

PhysicsObject* PhysicsSystem::CreatePhysicsObject(const glm::vec3& position, iShape* shape) {
	PhysicsObject* physicsObject = new PhysicsObject(position);
	physicsObject->pShape = shape;
	m_PhysicsObjects.push_back(physicsObject);
	return physicsObject;
}
int count = 0;
void PhysicsSystem::UpdateStep(float duration) {
	size_t numPlayerObjects = playerObjects.size();
	size_t numPhysicsObjects = m_PhysicsObjects.size();

	if (numPhysicsObjects == 0 || numPlayerObjects == 0)
		return;

	// Detect collisions
	PhysicsObject* physObjA, * physObjB;

	iShape* shapeA, * shapeB;

	bool collision;

	// Got this from https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
	for (int i = 0; i < numPlayerObjects - 1; i++) {
		physObjA = playerObjects[i];
		shapeA = physObjA->pShape;
		Triangle* triangle = (Triangle*)shapeA;

		glm::vec3 tri1V1 = triangle->A + physObjA->position;
		glm::vec3 tri1V2 = triangle->B + physObjA->position;
		glm::vec3 tri1V3 = triangle->C + physObjA->position;

		// Find the normal
		glm::vec3 U = glm::vec3(0);
		glm::vec3 V = glm::vec3(0);
		glm::vec3 faceNormal = glm::vec3(0);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		// Compute normal
		faceNormal = glm::cross(U, V);
		faceNormal = glm::normalize(faceNormal);

		// Get a point on the triangle
		glm::vec3 triPoint = (tri1V1 + tri1V2 + tri1V3) / 3.f;

		// Get plane equation "Ax + By + Cz + D = 0" Variables
		float pe1 = faceNormal.x;
		float pe2 = faceNormal.y;
		float pe3 = faceNormal.z;
		float pe4 = (-pe1 * triPoint.x - pe2 * triPoint.y - pe3 * triPoint.z);

		for (int j = 0; j < numPhysicsObjects; j++) {
			physObjB = m_PhysicsObjects[j];
			shapeB = physObjB->pShape;
			Triangle* triangle2 = (Triangle*)shapeB;

			glm::vec3 tri2V1 = triangle2->A;//+ physObjB->position;
			glm::vec3 tri2V2 = triangle2->B;//+ physObjB->position;
			glm::vec3 tri2V3 = triangle2->C;//+ physObjB->position;

			float ep1, ep2, t = 0.0f;
			float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
			glm::vec3 pointInPlane = glm::vec3(0);

			glm::vec3 t2V[4];
			t2V[0] = tri2V1;
			t2V[1] = tri2V2;
			t2V[2] = tri2V3;
			t2V[3] = tri2V1;

			for (int k = 0; k < 3; k++)
			{
				//Calculate where on the line, created from this trangles
				//edge (eg. tri2V1, tri2V2), intersects with the plane
				ep1 = (pe1 * t2V[k].x) + (pe2 * t2V[k].y) + (pe3 * t2V[k].z);
				ep2 = (pe1 * t2V[k + 1].x) + (pe2 * t2V[k + 1].y) + (pe3 * t2V[k + 1].z);

				//Set t to -1 in case there is a divide-by-zero
				t = -1;

				//Make sure there are no divide-by-zeros
				if (ep1 != ep2)
					t = -(ep2 + pe4) / (ep1 - ep2);

				//If the lines intersection with the triangles plane was between the
				//two vertices (tri2V1, tri2V2), calculate the point of intersection
				if (t >= 0.0f && t <= 1.0f)
				{
					planeIntersectX = (t2V[k].x * t) + (t2V[k + 1].x * (1 - t));
					planeIntersectY = (t2V[k].y * t) + (t2V[k + 1].y * (1 - t));
					planeIntersectZ = (t2V[k].z * t) + (t2V[k + 1].z * (1 - t));

					pointInPlane = glm::vec3(planeIntersectX, planeIntersectY, planeIntersectZ);
					count++;
					if (count > 150000)
					{
						count = 0;
						std::cout << "tri1v1 = " << tri1V1.x << " " << tri1V1.y << " " << tri1V1.z;
						std::cout << " tri1v2 = " << tri1V2.x << " " << tri1V2.y << " " << tri1V2.z;
						std::cout << " tri1v3 = " << tri1V3.x << " " << tri1V3.y << " " << tri1V3.z
							<< "\npointInPlane = " << pointInPlane.x << " " << pointInPlane.y << " " << pointInPlane.z << std::endl;
					}
					//Call function to check if point is in the triangle
					if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
					{
						if (physObjB->m_IsStatic == false)
						{
							physObjA->position = physObjA->prevPosition + glm::vec3(0.1f);// +glm::normalize(physObjA->position - avgPos) * 2.f;
							physObjA->SetForce(glm::normalize(triPoint - physObjA->position));
							//physObjA->KillAllForces();
							physObjA->velocity = glm::vec3(0);

							cMeshObject* colSpot;
							colSpot = new cMeshObject();
							colSpot->meshName = "ISO_Sphere_1";
							colSpot->friendlyName = std::to_string(triPoint.x + triPoint.y + triPoint.z);
							colSpot->bUse_RGBA_colour = true;
							colSpot->RGBA_colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.f);
							colSpot->isWireframe = true;
							colSpot->scale = 1.0f;
							colSpot->bDoNotLight = true;
							colSpot->position = triPoint;
							g_pMeshObjects.push_back(colSpot);
						}
					}
				}
				//collision = CollisionTest(physObjA->position, shapeA, physObjB->position, shapeB);

				//if (collision) {
				//	if (physObjA->m_IsStatic == false)
				//	{
				//		physObjA->position.y = physObjA->prevPosition.y;
				//		////physObjA->KillAllForces();
				//		physObjA->velocity.y = 0.0f;

				//		// Bounce:
				//		//physObjA->velocity = glm::vec3(0.0f) - physObjA->velocity * 0.8f;
				//	}

				//	if (physObjB->m_IsStatic == false)
				//	{
				//		physObjB->position.y = physObjB->prevPosition.y;
				//		//physObjB->KillAllForces();
				//		physObjB->velocity.y = 0.0f;

				//		//physObjB->velocity = glm::vec3(0.0f) - physObjB->velocity * 0.8f;
				//	}
				//}
			}
		}

		for (int i = 0; i < numPhysicsObjects; i++) {
			m_PhysicsObjects[i]->KillAllForces();
		}
	}
}

bool PhysicsSystem::PointInTriangle(glm::vec3 triV1, glm::vec3  triV2, glm::vec3  triV3, glm::vec3  point)
{
	//To find out if the point is inside the triangle, we will first find the area
	//of the entire triangle. After we find the area of the entire triangle, we will
	//use the point as a vertex, and create 3 more triangles using the three vertices
	//which make up the first triangle. We will find the area of the three triangles we
	//make using the point, then add the three triangle areas together. If the area
	//of the three triangles added up is the same as the first triangles area, the point
	//is inside the triangle. If the area of the three triangles added up is greater than
	//the area of the first triangle, the point is outside the triangle.

	//Find area of first triangle
	float distX = triV1.x - triV2.x;
	float distY = triV1.y - triV2.y;
	float distZ = triV1.z - triV2.z;

	float edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

	distX = triV1.x - triV3.x;
	distY = triV1.y - triV3.y;
	distZ = triV1.z - triV3.z;

	float edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

	distX = triV2.x - triV3.x;
	distY = triV2.y - triV3.y;
	distZ = triV2.z - triV3.z;

	float edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

	float s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

	float mainTriArea = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));

	//Find areas of the three triangles created with the point

	float smallTriArea[3] = { 0.0f, 0.0f, 0.0f };
	glm::vec3  triVert[4];
	triVert[0] = triV1;
	triVert[1] = triV2;
	triVert[2] = triV3;
	triVert[3] = triV1; //When i=2, i+1 will be triV1

	//Find 3 triangle areas using the plane intersecting point
	for (int i = 0; i < 3; i++)
	{
		distX = point.x - triVert[i].x;
		distY = point.y - triVert[i].y;
		distZ = point.z - triVert[i].z;

		edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

		distX = point.x - triVert[i + 1].x;
		distY = point.y - triVert[i + 1].y;
		distZ = point.z - triVert[i + 1].z;

		edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

		distX = triVert[i].x - triVert[i + 1].x;
		distY = triVert[i].y - triVert[i + 1].y;
		distZ = triVert[i].z - triVert[i + 1].z;

		edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

		s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

		smallTriArea[i] = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));
	}

	float totalSmallTriArea = smallTriArea[0] + smallTriArea[1] + smallTriArea[2];

	//Compare the three smaller triangle areas with the main triangles area
	//Subtract a small value from totalSmallTriArea to make up for inacuracy
	if (mainTriArea >= (totalSmallTriArea - 0.001f))
	{
		return true;
	}

	return false;
}

void PhysicsSystem::AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle)
{
	m_AABBStructure[hash].push_back(triangle);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, iShape* shapeA, const glm::vec3& posB, iShape* shapeB)
{
	bool detectedCollision = false;

	// ShapeA AABB Collision test
	if (shapeA->GetType() == SHAPE_TYPE_AABB)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<AABB*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {}
	}

	// ShapeA Sphere collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_SPHERE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<AABB*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Sphere*>(shapeB));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE) {
			detectedCollision = CollisionTest(posA, dynamic_cast<Sphere*>(shapeA), posB, dynamic_cast<Triangle*>(shapeB));
		}
	}

	// ShapeA Triangle collision tests
	else if (shapeA->GetType() == SHAPE_TYPE_TRIANGLE)
	{
		if (shapeB->GetType() == SHAPE_TYPE_AABB) {}

		else if (shapeB->GetType() == SHAPE_TYPE_SPHERE) {
			detectedCollision = CollisionTest(posB, dynamic_cast<Sphere*>(shapeB), posA, dynamic_cast<Triangle*>(shapeA));
		}

		else if (shapeB->GetType() == SHAPE_TYPE_TRIANGLE)
		{
			detectedCollision = CollisionTest(posA, dynamic_cast<Triangle*>(shapeA), posB, dynamic_cast<Triangle*>(shapeB));
		}
	}

	// ShapeA is unknown shape...
	else
	{
		// what is this!?!?!
	}

	return detectedCollision;
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Sphere* b)
{
	return TestSphereSphere(posA + a->Center, a->Radius, posB + b->Center, b->Radius);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, AABB* b)
{
	return TestSphereAABB(posA + a->Center, a->Radius, *b);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Triangle* b)
{
	Point unused;
	return TestSphereTriangle(posA + a->Center, a->Radius, posB + (*b).A, posB + (*b).B, posB + (*b).C, unused);
}

bool PhysicsSystem::CollisionTest(const glm::vec3& posA, Triangle* a, const glm::vec3& posB, Triangle* b)
{
	Point unused;
	return TestTriangleTriangle(a->A, a->B, a->C, b->A, b->B, b->C, unused);
}