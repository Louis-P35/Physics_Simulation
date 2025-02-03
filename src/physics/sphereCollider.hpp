#pragma once

// Includes from project
#include "collider.hpp"


/*
* Class SphereCollider
* 
* This class is used to detect collisions between a sphere and a line segment
*/
class SphereCollider : public Collider
{
private:
	double m_radius;

public:
	SphereCollider(const Vec3& position, const double radius);
	virtual ~SphereCollider();

	virtual bool hasCollided(
		Vec3& collPosition, 
		Vec3& collNormal,
		Vec3& bounceVect,
		const Vec3& p0, 
		const Vec3& p1,
		const double partRadius,
		const AABB& aabb
	) const override;
};