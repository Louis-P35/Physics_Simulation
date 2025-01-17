#pragma once

// Includes from project
#include "../src/math/vec3.hpp"


/*
* Class Collider
* 
* This is the base class to detect collisions between objects
*/
class Collider
{
protected:
	Vec3 m_position;

public:
	Collider(const Vec3& position);
	virtual ~Collider();

	virtual bool hasCollided(
		Vec3& collPosition, 
		Vec3& collNormal,
		Vec3& bounceVect,
		const Vec3& p0, 
		const Vec3& p1
	) const = 0;
};