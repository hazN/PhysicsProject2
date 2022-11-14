#pragma once

#include "PhysicsMath.h"
#include "Shapes.h"

int TestSphereSphere(const glm::vec3& posA, float radiusA, const glm::vec3& posB, float radiusB);
float SqDistPointAABB(glm::vec3 p, AABB b);
int TestSphereAABB(const glm::vec3& center, float radius, AABB b);
Point ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);
int TestSphereTriangle(const glm::vec3& center, float radius, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3& p);
float getSmallTriArea(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 point);
int TestTriangleTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 a2, glm::vec3 b2, glm::vec3 c2, glm::vec3& p);