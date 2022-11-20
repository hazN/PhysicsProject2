#include "PhysicsUtils.h"

int TestSphereSphere(const glm::vec3& posA, float radiusA, const glm::vec3& posB, float radiusB)
{
	// Calculate squared distance between centers
	glm::vec3 d = posA - posB;
	float dist2 = Dot(d, d);

	// Spheres intersect if squared distance is less
	// than squared sum of radii
	float radiusSum = radiusA + radiusB;
	return dist2 <= radiusSum * radiusSum;
}

float SqDistPointAABB(glm::vec3 p, AABB b)
{
	float sqDist = 0.0f;

	// x, y, z
	// 0, 1, 2

	//for (int i = 0; i < 3; i++) {
	//	// For each axis count any excess distance outside box extents
	//	float v = p[i];
	//	if (v < b.Min[i]) sqDist += (b.Min[i] - v) * (b.Min[i] - v);
	//	if (v > b.Max[i]) sqDist += (v - b.Max[i]) * (v - b.Max[i]);
	//}
	float v;
	v = p.x;
	if (v < b.Min[0]) sqDist += (b.Min[0] - v) * (b.Min[0] - v);
	if (v > b.Max[0]) sqDist += (v - b.Max[0]) * (v - b.Max[0]);

	v = p.y;
	if (v < b.Min[1]) sqDist += (b.Min[1] - v) * (b.Min[1] - v);
	if (v > b.Max[1]) sqDist += (v - b.Max[1]) * (v - b.Max[1]);

	v = p.z;
	if (v < b.Min[2]) sqDist += (b.Min[2] - v) * (b.Min[2] - v);
	if (v > b.Max[2]) sqDist += (v - b.Max[2]) * (v - b.Max[2]);

	return sqDist;
}

int TestSphereAABB(const glm::vec3& center, float radius, AABB b)
{
	// Compute squared distance between sphere center and AABB
	float sqDist = SqDistPointAABB(center, b);

	// Sphere and AABB intersect if the (squared) distance
	// between them is less than the (squared) sphere radius
	return sqDist <= radius * radius;
}

Point ClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
	// Check if P in vertex region outside A
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;
	float d1 = Dot(ab, ap);
	float d2 = Dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return a;
	// Barycentric coordinates (1, 0, 0)

	// Check if P in vertex region outside B
	glm::vec3 bp = p - b;
	float d3 = Dot(ab, bp);
	float d4 = Dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return b;
	// barycentric coordinates (0,1,0)

	// Check if P in edge region of AB, if so return projection of P
	//onto AB
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + v * ab; // barycentric coordinates (1-v, v, 0)
	}

	// Check if P in vertex region outside C
	glm::vec3 cp = p - c;
	float d5 = Dot(ab, cp);
	float d6 = Dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return c;
	// barycentric coordinates (0, 0, 1)

	// Check if P in edge region of AC, if so return projection of P
	// onto AC
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + w * ac; // barycentric coordinates (1-w, 0, w)
	}

	// Check if P in edge region of BC, if so return projection of P
	// onto BC
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b);
		// barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric
	// coordinates(u, v, w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return a + ab * v + ac * w;
	// = u*a +v*b + w*c, u = va * denom = 1.0f - v - w
}

int TestSphereTriangle(const Point& center, float radius, Point a, Point b, Point c, Point& p)
{
	// Find point P on triangle ABC closest to the sphere center
	p = ClosestPtPointTriangle(center, a, b, c);
	// Sphere and triangle intersect if the (squared) distance from sphere
	// center to point p is less than the (squared) sphere radius
	glm::vec3 v = p - center;
	return Dot(v, v) <= radius * radius;
}

float getSmallTriArea(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 point)
{
	float distX;
	float distY;
	float distZ;
	float edgeLength1;
	float edgeLength2;
	float edgeLength3;
	float s;
	// Create 3 Triangles and find their area

	float smallTriArea[3] = { 0.0f, 0.0f, 0.0f };
	glm::vec3 triVert[4];
	triVert[0] = a;
	triVert[1] = b;
	triVert[2] = c;
	triVert[3] = a;

	// Find 3 triangle areas using the plane intersecting point
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

	return smallTriArea[0] + smallTriArea[1] + smallTriArea[2];
}

//https://www.braynzarsoft.net/viewtutorial/q16390-24-triangle-to-triangle-collision-detection
int TestTriangleTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 a2, glm::vec3 b2, glm::vec3 c2, glm::vec3& p)
{
	// Find area of first triangle
	{
		// Edge 1
		float distX = a.x - b.x;
		float distY = a.y - b.y;
		float distZ = a.z - b.z;

		float edgeLength1 = sqrt(distX * distX + distY * distY + distZ * distZ);

		// Edge 2
		distX = a.x - c.x;
		distY = a.y - c.y;
		distZ = a.z - c.z;

		float edgeLength2 = sqrt(distX * distX + distY * distY + distZ * distZ);

		// Edge 3
		distX = b.x - c.x;
		distY = b.y - c.y;
		distZ = b.z - c.z;

		float edgeLength3 = sqrt(distX * distX + distY * distY + distZ * distZ);

		float s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

		float Area1 = sqrt(s * (s - edgeLength1) * (s - edgeLength2) * (s - edgeLength3));

		float totalSmallTriArea = getSmallTriArea(a, b, c, a2);

		// Compare the three smaller triangles with the first triangles area
		// Subtract a small value to make up for inaccuracy
		if (Area1 >= (totalSmallTriArea - 0.001f))
		{
			return true;
		}
		totalSmallTriArea = getSmallTriArea(a, b, c, b2);
		if (Area1 >= (totalSmallTriArea - 0.001f))
		{
			return true;
		}
		totalSmallTriArea = getSmallTriArea(a, b, c, c2);
		if (Area1 >= (totalSmallTriArea - 0.001f))
		{
			return true;
		}
		return false;
	}
}