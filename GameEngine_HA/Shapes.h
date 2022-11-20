#pragma once

#include "PhysicsMath.h"

#include <memory>
#include <string>

enum ShapeType {
	SHAPE_TYPE_SPHERE,
	SHAPE_TYPE_AABB,
	SHAPE_TYPE_TRIANGLE
};

class iShape {
public:
	virtual ShapeType GetType() const = 0;
	virtual std::string GetOwner() = 0;
};

class Sphere : public iShape {
public:
	Sphere(const Point& center, float radius)
		: Center(center)
		, Radius(radius)
	{ }

	virtual ~Sphere()
	{ }

	virtual ShapeType GetType() const override {
		return SHAPE_TYPE_SPHERE;
	}
	virtual std::string GetOwner() override
	{
		return Owner;
	}
	Point Center;
	float Radius;
	std::string Owner;
};

class AABB : public iShape {
public:
	AABB(float min[3], float max[3]) {
		memcpy(&(Min[0]), &(min[0]), 3 * sizeof(float));
		memcpy(&(Max[0]), &(max[0]), 3 * sizeof(float));
	}
	virtual ~AABB() {
	}

	virtual ShapeType GetType() const override {
		return SHAPE_TYPE_AABB;
	}
	virtual std::string GetOwner() override
	{
		return Owner;
	}
	// x, y, z will be indexed as 0, 1, 2 respectively
	float Min[3];
	float Max[3];
	std::string Owner;
};

class Triangle : public iShape {
public:
	Triangle(Point a, Point b, Point c)
		: A(a), B(b), C(c) { }
	virtual ~Triangle() { }

	virtual ShapeType GetType() const override {
		return SHAPE_TYPE_TRIANGLE;
	}
	virtual std::string GetOwner() override
	{
		return Owner;
	}
	Point A;
	Point B;
	Point C;
	std::string Owner;
	glm::vec3 avgPos;
};