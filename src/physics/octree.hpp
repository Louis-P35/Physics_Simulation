#pragma once

// Includes from project
#include "../src/physics/aabb.hpp"
#include "../src/math/vec3.hpp"

// Includes from STL
#include <memory>
#include <vector>


class OctreeNode
{
public:
	std::vector<std::shared_ptr<OctreeNode>> m_pChildren;

	AABB m_aabb;

	std::vector<std::array<Vec3, 3>> m_triangles;

private:
	OctreeNode* m_pParent = nullptr;
	

public:
	OctreeNode(const Vec3& min, const Vec3& max) : m_aabb(min, max) {};
	OctreeNode(const AABB& other) : m_aabb(other) {};
	OctreeNode(const std::vector<AABB>& others) : m_aabb(others) {};
	~OctreeNode() {};

	bool isLeaf() const;
	void addChildren(const Vec3& min, const Vec3& max);
	void addChildren(std::shared_ptr<OctreeNode> pChild);
	std::vector<OctreeNode*> detectCollision(const Vec3& p0, const Vec3& p1) const;
	std::vector<OctreeNode*> detectCollision(const AABB& other) const;

private:
	template <typename CollisionLambda>
	std::vector<OctreeNode*> detectCollisionInternal(CollisionLambda collisionLambda) const;
};