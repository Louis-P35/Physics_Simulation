// Includes from project
#include "aabb.hpp"

// Includes from STL
#include <algorithm>


/*
* Construct a cubic AABB from a position and half size
* 
* @param pos The center position of the AABB
* @param halfSize The half lenght of one side of the cube (cubic AABB)
* @return void
*/
void AABB::constructAABB(const Vec3& pos, const double halfSize)
{
	m_min = pos - Vec3(halfSize, halfSize, halfSize);
	m_max = pos + Vec3(halfSize, halfSize, halfSize);
}


/*
* Detect if the AABB has collided with another AABB
* 
* @param other The other AABB
* @return bool True if the AABB has collided with the other AABB, false otherwise
*/
bool AABB::hasCollided(const AABB& other) const
{
	if (m_max.x < other.m_min.x || m_min.x > other.m_max.x)
	{
		return false;
	}
	if (m_max.y < other.m_min.y || m_min.y > other.m_max.y)
	{
		return false;
	}
	if (m_max.z < other.m_min.z || m_min.z > other.m_max.z)
	{
		return false;
	}

	return true;
}


/*
* Detect if the AABB has collided with a line segment
* 
* @param p0 Start of the line segment
* @param p1 End of the line segment
* @return bool True if the AABB has collided with the line segment, false otherwise
*/
bool AABB::hasCollided(const Vec3& p0, const Vec3& p1) const
{
    // Direction vector of the ray
    Vec3 dir = p1 - p0;

    // Avoid division by zero by replacing zero with a very small number
    Vec3 invDir(
        dir.x != 0.0 ? 1.0 / dir.x : std::numeric_limits<double>::max(),
        dir.y != 0.0 ? 1.0 / dir.y : std::numeric_limits<double>::max(),
        dir.z != 0.0 ? 1.0 / dir.z : std::numeric_limits<double>::max()
    );

    // Compute intersection t-values for each axis
    double t1 = (m_min.x - p0.x) * invDir.x;
    double t2 = (m_max.x - p0.x) * invDir.x;

    double t3 = (m_min.y - p0.y) * invDir.y;
    double t4 = (m_max.y - p0.y) * invDir.y;

    double t5 = (m_min.z - p0.z) * invDir.z;
    double t6 = (m_max.z - p0.z) * invDir.z;

    // Find the largest t_min and the smallest t_max
    double t_min = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
    double t_max = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });

    // If t_max < 0, the ray is intersecting the AABB, but the box is behind the ray
    if (t_max < 0)
    {
        return false;
    }

    // If t_min > t_max, there is no intersection
    if (t_min > t_max)
    {
        return false;
    }

    // There is an intersection
    return true;
}