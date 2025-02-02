#pragma once

// Includes from project
#include "../src/math/vec3.hpp"

class Ray
{
public:
	Vec3 origin;
	Vec3 direction;
	Ray(const Vec3& origin, const Vec3& direction) : origin(origin), direction(direction) {};
	Ray() {};
};

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
		const Vec3& p1,
		const double partRadius
	) const = 0;
};