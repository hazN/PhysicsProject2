#pragma once

#include <vector>

#include "BoundingBox.h"
#include "Shapes.h"

class PhysicsObject {
	// Private area
public:
	PhysicsObject();
	PhysicsObject(const glm::vec3& position);
	~PhysicsObject();
	PhysicsObject(const PhysicsObject& particle);
	PhysicsObject operator=(const PhysicsObject& particle);

	void SetMass(float mass) {
		// If the mass is 0 or less we treat it as
		// a static
		if (mass <= 0) {
			m_IsStatic = true;
			invMass = -1.f;
		}

		m_IsStatic = false;
		invMass = 1.f / mass;
	}

	void Integrate(float dt);
	void ApplyForce(const glm::vec3& direction);
	void KillAllForces();
	void SetForce(const glm::vec3& direction);

	inline const glm::vec3& GetPosition() const { return position; }
	inline const glm::vec3& GetVelocity() const { return velocity; }
	inline const glm::vec3& GetAcceleration() const { return acceleration; }
	std::vector<Triangle*> triangles;

	BoundingBox* pBoundingBox;
	iShape* pShape;

	friend class PhysicsSystem;

	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	glm::vec3 force;
	float damping;

	bool m_IsStatic;
	float invMass;

	void PrintInfo();
};