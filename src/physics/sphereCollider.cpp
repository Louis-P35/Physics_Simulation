// Includes from project
#include "sphereCollider.hpp"

// Includes from STL
#include <iostream>

SphereCollider::SphereCollider(const Vec3& position, const double radius) : Collider(position), m_radius(radius)
{
	
}


SphereCollider::~SphereCollider()
{

}


/*
* Check if a sphere has collided with a line segment
* 
* @param collPosition Position of the collision
* @param collNormal Normal of the collision
* @param bounceVect Bounce vector (direction of the bounce)
* @param p0 Start of the line segment
* @param p1 End of the line segment
* @return bool True if the sphere has collided with the line segment, false otherwise
*/
bool SphereCollider::hasCollided(Vec3& collPosition, Vec3& collNormal, Vec3& bounceVect, const Vec3& p0, const Vec3& p1, const double partRadius) const
{
	// Compute the distance between the sphere center and the p1 point
	const double d = (m_position - p1).norm();

	if (d <= (m_radius + partRadius))
	{
		// The sphere has collided with the line segment
		collNormal = (p1 - m_position).getNormalized();
		collPosition = m_position + collNormal * m_radius;

		// Compute the bounce vector
		const Vec3 v = (p1 - p0).getNormalized();
		bounceVect = Collider::getBounceVector(v, collNormal);

		return true;
	}

	return false;
}