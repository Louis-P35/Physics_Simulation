#pragma once

// Includes from project
#include "../src/math/vec3.hpp"

// Includes from STL
#include <vector>
#include <memory>


/*
* Class AABB
* 
* This class is used to represent an Axis Aligned Bounding Box
* It is used to detect collisions between two AABBs
* and to detect collision between an AABB and a ray
*/
class AABB
{
public:
	Vec3 m_min;
	Vec3 m_max;

public:
	double m_halfSize = -1;

public:
	AABB(const Vec3& min, const Vec3& max) : m_min(min), m_max(max) {}
	AABB(const AABB& other) : m_min(other.m_min), m_max(other.m_max), m_halfSize(other.m_halfSize) {}
	AABB(const std::vector<AABB>& others);
	AABB(const std::vector<std::shared_ptr<AABB>>& others);
	AABB(const double halfSize) : m_halfSize(halfSize) {};
	~AABB() {};
	void setAabb(const AABB& other);
	void setAabb(const std::vector<AABB>& others);
	void setAabb(const std::vector<std::shared_ptr<AABB>>& others);
	bool hasCollided(const AABB& other) const;
	bool hasCollided(const Vec3& p0, const Vec3& p1) const;
	void constructCubicAABB(const Vec3& pos, const double halfSize);
	void constructCubicAABB(const Vec3& pos);
};