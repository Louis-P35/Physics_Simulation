#pragma once

// Includes from project
#include "../src/physics/aabb.hpp"
#include "../src/math/vec3.hpp"

// Includes from STL
#include <memory>
#include <vector>


class OctreeNode : public AABB
{
private:
	std::vector<std::shared_ptr<OctreeNode>> m_pChildren;
	OctreeNode* m_pParent = nullptr;
	

public:
	OctreeNode(const Vec3& min, const Vec3& max) : AABB(min, max) {}
	~OctreeNode() {};

	bool isLeaf() const;
	void addChildren(const Vec3& min, const Vec3& max);
	std::vector<OctreeNode*> detectCollision(const Vec3& p0, const Vec3& p1) const;
	std::vector<OctreeNode*> detectCollision(const AABB& other) const;

private:
	template <typename CollisionLambda>
	std::vector<OctreeNode*> detectCollisionInternal(CollisionLambda collisionLambda) const;
};