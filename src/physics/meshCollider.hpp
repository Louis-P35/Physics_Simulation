#pragma once

// Includes from project
#include "../src/physics/collider.hpp"
#include "../src/physics/octree.hpp"
#include "../src/view/OpenGl/object3D.hpp"

// Includes from STL
#include <memory>
#include <vector>
#include <array>


/*
* Class MeshCollider
* Is used to detect collisions between a mesh and a line segment
* Use a tree structure to optimize the collision detection
*/
class MeshCollider : public Collider
{
private:
	std::shared_ptr<OctreeNode> m_pRoot;
	std::vector<std::array<Vec3, 3>> m_triangles;


public:
	MeshCollider(const Vec3& position, const Object3D& obj);
	virtual ~MeshCollider() {};

	virtual bool hasCollided(
		Vec3& collPosition,
		Vec3& collNormal,
		Vec3& bounceVect,
		const Vec3& p0,
		const Vec3& p1,
		const double partRadius
	) const override;

	static bool rayTriangleIntersection(
		const Ray& ray, 
		const Vec3& v0, const Vec3& v1, const Vec3& v2, 
		Vec3& collPosition, 
		Vec3& collNormal
	);

	static Vec3 ClosestPointOnTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& point);
	static Vec3 ClosestPointOnLineSegment(const Vec3& a, const Vec3& b, const Vec3& point);
};