#include "PhysicsSystem.h"
#include "PhysicsUtils.h"

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


void PhysicsSystem::UpdateStep(float duration) {
	size_t numPhysicsObjects = m_PhysicsObjects.size();

	if (numPhysicsObjects == 0)
		return;

	for (int i = 0; i < numPhysicsObjects; i++) {
		/* Gravity */
		m_PhysicsObjects[i]->ApplyForce(glm::vec3(0, -9.81f, 0));
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->Integrate(duration);

	}

	// Detect collisions
	PhysicsObject* physObjA, * physObjB;

	iShape* shapeA, * shapeB;

	bool collision;

	for (int i = 0; i < numPhysicsObjects - 1; i++) {
		physObjA = m_PhysicsObjects[i];
		shapeA = physObjA->pShape;

		for (int j = i + 1; j < numPhysicsObjects; j++) {
			physObjB = m_PhysicsObjects[j];
			shapeB = physObjB->pShape;

			collision = CollisionTest(physObjA->position, shapeA, physObjB->position, shapeB);

			if (collision) {
				if (physObjA->m_IsStatic == false)
				{
					physObjA->position.y = physObjA->prevPosition.y;
					////physObjA->KillAllForces();
					physObjA->velocity.y = 0.0f;


					// Bounce:
					//physObjA->velocity = glm::vec3(0.0f) - physObjA->velocity * 0.8f;
				}

				if (physObjB->m_IsStatic == false)
				{
					physObjB->position.y = physObjB->prevPosition.y;
					//physObjB->KillAllForces();
					physObjB->velocity.y = 0.0f;

					//physObjB->velocity = glm::vec3(0.0f) - physObjB->velocity * 0.8f;
				}
			}
		}
	}

	for (int i = 0; i < numPhysicsObjects; i++) {
		m_PhysicsObjects[i]->KillAllForces();
	}
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
	return TestTriangleTriangle(a->A,a->B,a->C,b->A,b->B,b->C, unused);
}
