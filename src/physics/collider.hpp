#pragma once

// Includes from project
#include "../src/math/vec3.hpp"
#include "../src/physics/aabb.hpp"

class Ray
{
public:
	Vec3 m_origin;
	Vec3 m_direction;
	double m_lenght;
	Ray(const Vec3& origin, const Vec3& direction, const double lenght) : m_origin(origin), m_direction(direction), m_lenght(lenght){};
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
	Vec3 m_colliderPosition;

public:
	Collider(const Vec3& position);
	virtual ~Collider();

	virtual bool hasCollided(
		Vec3& collPosition, 
		Vec3& collNormal,
		Vec3& bounceVect,
		const Vec3& p0, 
		const Vec3& p1,
		const double partRadius,
		const AABB& aabb
	) const = 0;


	/*
	* Compute the bounce vector
	* inline, so defined here
	*
	* @param particleDir The vector to bounce
	* @param collNormal The normal of the collision
	* @return Vec3 The bounce vector
	*/
	inline static Vec3 getBounceVector(const Vec3& particleDir, const Vec3& collNormal)
	{
		return particleDir - collNormal * (2.0 * particleDir.dot(collNormal));
	};
};