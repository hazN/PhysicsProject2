#include "PhysicsSystem.h"
#include "PhysicsUtils.h"
#include <DirectXMath.h>
#include <iostream>
#include <thread>
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

// Method : Initialize
// Summary: Initializes the physics system
// Params : void
// Returns: void
void PhysicsSystem::Initialize()
{
	float scale = g_pMeshObjects[2]->scale;
	glm::vec3 position = g_pMeshObjects[2]->position;
	std::vector<glm::vec3> vertices;
	std::vector<int> triangles;

	sModelDrawInfo draw_info;
	// Load the player object
	pVAOManager->FindDrawInfoByModelName("Warrior", draw_info);
	playerObject = new PhysicsObject;
	playerObject->m_IsStatic = false;
	for (int i = 0; i < draw_info.numberOfVertices; i++)
	{
		vertices.push_back(glm::vec3(draw_info.pVertices[i].x, draw_info.pVertices[i].y, draw_info.pVertices[i].z));
	}
	// Create our mesh inside the physics system
	for (int i = 0; i < draw_info.numberOfTriangles; i++) {
		glm::vec3 vertexA = (glm::vec3(vertices[draw_info.pTriangles[i]->vertexIndices[0]]) * scale);//+ position;
		glm::vec3 vertexB = (glm::vec3(vertices[draw_info.pTriangles[i]->vertexIndices[1]]) * scale);//+ position;
		glm::vec3 vertexC = (glm::vec3(vertices[draw_info.pTriangles[i]->vertexIndices[2]]) * scale);//+ position;

		Triangle* triangle = new Triangle(vertexA, vertexB, vertexC);
		triangle->Owner = "Warrior";
		//PhysicsObject* trianglePhysObj = m_PhysicsSystem.CreatePhysicsObject(glm::vec3(0), triangle);
		playerObject->triangles.push_back(triangle);
		//trianglePhysObj->SetMass(-1.f);
	}

	for (cMeshObject* obj : g_pMeshObjects)
	{
		if (obj->meshName == "Warrior" || obj->meshName == "ISO_Sphere_1" || obj->meshName == "ISO_Sphere_2")
		{
			continue;
		}
		pVAOManager->createPhysicsObject(obj->meshName, obj->position, obj->scale);
	}
	int quarter = (int)(playerObject->triangles.size() / 2) / 2;
	int half = (int)(playerObject->triangles.size() / 2);
	std::thread th([&]() {
		while (!endThread) {
			m_PhysicsSystem.UpdateStep(0, playerObject->triangles.size());
		}
		});
	th.detach();
	Loaded = true;
}

int count = 0;
float deltaTime = std::clock();
float duration = 0;
// Method : UpdateStep
// Summary: Loops through the playerobject and takes hash to check for collisions
// Params : int int
// Returns: void
// SOURCE : https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
void PhysicsSystem::UpdateStep(int min, int max) {
	size_t numPlayerObjects = playerObject->triangles.size();
	//size_t numPhysicsObjects = m_PhysicsObjects.size();

	if (numPlayerObjects == 0)
		return;
	if (max == numPlayerObjects)
	{
		max--;
	}
	// Detect collisions
	PhysicsObject* physObjA = playerObject;

	iShape* shapeA, * shapeB;

	bool collision;

	for (int i = min; i < max; i++) {
		shapeA = physObjA->triangles[i];
		Triangle* triangle = (Triangle*)shapeA;

		glm::vec3 tri1V1 = triangle->A + playerObject->position;
		glm::vec3 tri1V2 = triangle->B + playerObject->position;
		glm::vec3 tri1V3 = triangle->C + playerObject->position;
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

		int hashA = pVAOManager->CalculateHashValue(tri1V1);
		int hashB = pVAOManager->CalculateHashValue(tri1V2);
		int hashC = pVAOManager->CalculateHashValue(tri1V3);
		if (AABBloop(hashA, pe1, pe2, pe3, pe4, tri1V1, tri1V2, tri1V3, physObjA, triPoint))
			return;
		if (AABBloop(hashB, pe1, pe2, pe3, pe4, tri1V1, tri1V2, tri1V3, physObjA, triPoint))
			return;
		if (AABBloop(hashC, pe1, pe2, pe3, pe4, tri1V1, tri1V2, tri1V3, physObjA, triPoint))
			return;
	}
}
// Method : AABBloop
// Summary: Loops through AABBstructure at given hash value
// Params : int, float, float, float, float, float, vec3, vec3, vec3, PhysicsObject*, vec3
// Returns: bool
// SOURCE : https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
bool PhysicsSystem::AABBloop(int hash, float pe1, float pe2, float pe3, float pe4, glm::vec3 tri1V1, glm::vec3 tri1V2, glm::vec3 tri1V3, PhysicsObject* physObjA, glm::vec3 triPoint)
{
	for (int j = 0; j < m_AABBStructure[hash].size(); j++) {
		//PhysicsObject* physObjB;
		Triangle* shapeB = m_AABBStructure[hash].at(j);
		//shapeB = physObjB->pShape;
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
			// Calculate where on the line
			// edge (eg. tri2V1, tri2V2), intersects with the plane
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
				/*if (glm::length(pointInPlane - tri1V1) < 0.5f || glm::length(pointInPlane - tri1V2) < 0.5f || glm::length(pointInPlane - tri1V3) < 0.5f)
				{
					//std::cout << " close " << std::endl;/*
					count++;
					if (count > 150000)
					{
						count = 0;
						std::cout << "tri1v1 = " << tri1V1.x << " " << tri1V1.y << " " << tri1V1.z;
						std::cout << " tri1v2 = " << tri1V2.x << " " << tri1V2.y << " " << tri1V2.z;
						std::cout << " tri1v3 = " << tri1V3.x << " " << tri1V3.y << " " << tri1V3.z
							<< "\npointInPlane = " << pointInPlane.x << " " << pointInPlane.y << " " << pointInPlane.z << std::endl;
					}*/
					//Call function to check if point is in the triangle
				if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
				{
					/*physObjA->KillAllForces();
					physObjA->velocity = glm::vec3(0);
					physObjA->acceleration = glm::vec3(0);*/
					playerObject->KillAllForces();
					playerObject->position += (glm::normalize(playerObject->prevPosition - pointInPlane));// / 2.f);// *2.f;
					playerObject->SetForce(playerObject->force - glm::normalize(pointInPlane - physObjA->position));
					//physObjA->position = physObjA->prevPosition +glm::normalize(physObjA->position - pointInPlane) * 2.f;
					//physObjA->SetForce(glm::normalize(triPoint - physObjA->position));

					duration = (std::clock() - deltaTime) / (double)CLOCKS_PER_SEC;
					if (duration > 0.5f)
					{
						deltaTime = std::clock();
						cMeshObject* colSpot;
						colSpot = new cMeshObject();
						colSpot->meshName = "ISO_Sphere_1";
						colSpot->friendlyName = std::to_string(triPoint.x + triPoint.y + triPoint.z + rand() % 200);
						colSpot->bUse_RGBA_colour = true;
						colSpot->RGBA_colour = glm::vec4(0.0f, 1.0f, 0.0f, 1.f);
						colSpot->isWireframe = true;
						colSpot->scale = 1.0f;
						colSpot->bDoNotLight = true;
						colSpot->position = pointInPlane;
						g_pMeshObjects.push_back(colSpot);
					}
					return true;
				}
				//}
			}
		}
	}
	return false;
}
// Method : PointInTriangle
// Summary: Finds if a given point is inside given triangle by utilizing the area of the triangle
// Params : vec3, vec3, vec3, vec3
// Returns: bool
// SOURCE : https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
bool PhysicsSystem::PointInTriangle(glm::vec3 triV1, glm::vec3  triV2, glm::vec3  triV3, glm::vec3  point)
{
	// Edge 1
	float distX = triV1.x - triV2.x;
	float distY = triV1.y - triV2.y;
	float distZ = triV1.z - triV2.z;

	float edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

	// Edge 2
	distX = triV1.x - triV3.x;
	distY = triV1.y - triV3.y;
	distZ = triV1.z - triV3.z;

	float edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

	// Edge 3
	distX = triV2.x - triV3.x;
	distY = triV2.y - triV3.y;
	distZ = triV2.z - triV3.z;

	float edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

	// Get area of the triangle
	float s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

	float mainTriArea = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));

	// Find areas of the 3 triangles created with the pointInPlane

	float smallTriArea[3] = { 0.0f, 0.0f, 0.0f };
	glm::vec3  triVert[4];
	triVert[0] = triV1;
	triVert[1] = triV2;
	triVert[2] = triV3;
	triVert[3] = triV1; 

	// Find area of the 3 triangles using the pointInPlane
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

	// Check if the area of the smaller triangles are larger than the original triangle(aka collides)
	if (mainTriArea >= (totalSmallTriArea - 0.001f))
	{
		return true;
	}
	// No Collision
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