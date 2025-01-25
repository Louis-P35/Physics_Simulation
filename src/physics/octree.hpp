#pragma once

// Includes from project
#include "../src/physics/aabb.hpp"
#include "../src/math/vec3.hpp"

// Includes from STL
#include <memory>
#include <vector>


class OctreeNode : public AABB
{
public:
	std::vector<std::shared_ptr<OctreeNode>> m_pChildren;

	// Dirty
	int m_indexI = -1;
	int m_indexJ = -1;


private:
	OctreeNode* m_pParent = nullptr;
	

public:
	OctreeNode(const Vec3& min, const Vec3& max) : AABB(min, max) {};
	OctreeNode(const AABB& other) : AABB(other) {};
	OctreeNode(const std::vector<AABB>& others) : AABB(others) {};
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