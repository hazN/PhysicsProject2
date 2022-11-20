#pragma once

#include "PhysicsObject.h"

#include <map>
#include <vector>
#include "Shapes.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	~PhysicsSystem();

	PhysicsObject* CreatePhysicsObject(const glm::vec3& position, iShape* shape);
	void Initialize();
	void UpdateStep(int min, int max);
	void AddTriangleToAABBCollisionCheck(int hash, Triangle* triangle);
	bool AABBloop(int hash, float pe1, float pe2, float pe3, float pe4, glm::vec3 tri1V1, glm::vec3 tri1V2, glm::vec3 tri1V3, PhysicsObject* physObjA, glm::vec3 triPoint);

	const std::map<int, std::vector<Triangle*>>& GetAABBStructure() {
		return m_AABBStructure;
	}
	std::vector<PhysicsObject*> m_PhysicsObjects;
	//std::vector<PhysicsObject*> playerObjects;
	bool PointInTriangle(glm::vec3 triV1, glm::vec3 triV2, glm::vec3 triV3, glm::vec3 point);
	bool CollisionTest(const glm::vec3& posA, iShape* shapeA, const glm::vec3& posB, iShape* shapeB);
	bool CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Sphere* b);
	bool CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, AABB* b);
	bool CollisionTest(const glm::vec3& posA, Sphere* a, const glm::vec3& posB, Triangle* b);
	bool CollisionTest(const glm::vec3& posA, Triangle* a, const glm::vec3& posB, Triangle* b);
	//bool CollisionTest(AABB* a, Triangle* b);		// Not implementing
	//bool CollisionTest(AABB* a, AABB* b);			// Not implementing
	//bool CollisionTest(Triangle* a, Triangle* b);	// Not implementing

	std::map<int, std::vector<Triangle*>> m_AABBStructure;
};