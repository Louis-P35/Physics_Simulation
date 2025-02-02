// Includes from project
#include "meshCollider.hpp"

// Includes from STL
#include <cmath>
#include <algorithm>



MeshCollider::MeshCollider(const Vec3& position, const Object3D& obj) : Collider(position)
{
	for (size_t i = 0; i < obj.m_faces.size(); i++)
	{
        Vec3 v0 = Vec3(obj.m_vertices[obj.m_faces[i][0]]);
        Vec3 v1 = Vec3(obj.m_vertices[obj.m_faces[i][3]]);
        Vec3 v2 = Vec3(obj.m_vertices[obj.m_faces[i][6]]);

		m_triangles.push_back({v0, v1, v2});
	}
}


/*
* Check if a mesh has collided with a line segment
*
* @param collPosition Position of the collision
* @param collNormal Normal of the collision
* @param bounceVect Bounce vector (direction of the bounce)
* @param p0 Start of the line segment
* @param p1 End of the line segment
* @return bool True if the mesh has collided with the line segment, false otherwise
*/
bool MeshCollider::hasCollided(
	Vec3& collPosition,
	Vec3& collNormal,
	Vec3& bounceVect,
	const Vec3& p0,
	const Vec3& p1,
	const double partRadius
) const
{
	double length = (p1 - p0).norm();
	Vec3 particleDir = (p1 - p0).getNormalized();
	Vec3 particlePos = p0 - m_position;
	Ray ray(particlePos, particleDir, length);

	double closestTriangleDist = std::numeric_limits<double>::max();
    bool hasCollided = false;
    Vec3 closestClosestPointOnT;
    Vec3 closestNormalOnT;
	for (const auto& triangle : m_triangles)
	{
		Vec3 closestPointOnT = ClosestPointOnTriangle(triangle[0], triangle[1], triangle[2], particlePos);
		double distToT = (closestPointOnT - particlePos).norm();
        if (distToT < partRadius)
        {
			hasCollided = true;
			if (distToT < closestTriangleDist)
			{
				closestTriangleDist = distToT;
                closestClosestPointOnT = closestPointOnT;
			}
        }
	}

    if (hasCollided)
    {
        collPosition = closestClosestPointOnT + m_position;
        collNormal = (particlePos - closestClosestPointOnT).getNormalized();

        // Compute the bounce vector
        const Vec3 v = (p1 - p0).getNormalized();
        bounceVect = Collider::getBounceVector(v, collNormal);

        return true;
    }

	return false;
}


/*
* Detect if the input ray has collided with the mesh
* Use the Möller–Trumbore algorithm
* 
* @param ray The input ray
* @param v0 The first vertex of the triangle
* @param v1 The second vertex of the triangle
* @param v2 The third vertex of the triangle
* @param collPosition The position of the collision
* @param collNormal The normal of the collision
* @return bool True if the ray has collided with the triangle, false otherwise
*/
bool MeshCollider::rayTriangleIntersection(
    const Ray& ray, const Vec3& v0, const Vec3& v1, const Vec3& v2, 
    Vec3& collPosition, 
    Vec3& collNormal
)
{
    constexpr double EPSILON = 1e-6;

    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    collNormal = edge1.cross(edge2).getNormalized();  // Precompute triangle normal

    Vec3 h = ray.m_direction.cross(edge2);
    double a = edge1.dot(h);

    if (fabs(a) < EPSILON)
    {
        return false;  // Parallel case (no intersection)
    }

    double f = 1.0 / a;
    Vec3 s = ray.m_origin - v0;
    double u = f * s.dot(h);

    if (u < 0.0 || u > 1.0)
    {
        return false;  // Barycentric coordinate check
    }

    Vec3 q = s.cross(edge1);
    double v = f * ray.m_direction.dot(q);

    if (v < 0.0 || (u + v) > 1.0)
    {
        return false;  // Barycentric coordinate check
    }

    double t = f * edge2.dot(q);

    if (t > EPSILON) // Intersection valid if t is positive
    {  
        collPosition = ray.m_origin + ray.m_direction * t;
        return true;
    }

    return false;  // Intersection behind the ray origin
}


/*
* Compute the closest point on a line segment to a point
* 
* @param a Start of the line segment
* @param b End of the line segment
* @param point The point
* @return Vec3 The closest point on the line segment
*/
Vec3 MeshCollider::ClosestPointOnLineSegment(const Vec3& a, const Vec3& b, const Vec3& point)
{
    Vec3 ab = b - a;
    double t = (point - a).dot(ab) / ab.dot(ab);
    t = std::clamp(t, 0.0, 1.0);  // Clamp to segment
    return a + ab * t;
}


/*
* Compute the closest point on a triangle to a point
* 
* @param v0 The first vertex of the triangle
* @param v1 The second vertex of the triangle
* @param v2 The third vertex of the triangle
* @param point The point
* @return Vec3 The closest point on the triangle
*/
Vec3 MeshCollider::ClosestPointOnTriangle(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& point)
{
    Vec3 edge0 = v1 - v0;
    Vec3 edge1 = v2 - v0;
    Vec3 v0ToPoint = point - v0;

    double d00 = edge0.dot(edge0);
    double d01 = edge0.dot(edge1);
    double d11 = edge1.dot(edge1);
    double d20 = v0ToPoint.dot(edge0);
    double d21 = v0ToPoint.dot(edge1);

    double denom = d00 * d11 - d01 * d01;
    double v = (d11 * d20 - d01 * d21) / denom;
    double w = (d00 * d21 - d01 * d20) / denom;
    double u = 1.0 - v - w;

    // Check if the point is inside the triangle
    if (u >= 0 && v >= 0 && w >= 0)
    {
        return v0 * u + v1 * v + v2 * w;  // Closest point is inside
    }

    // Otherwise, check closest edge
    Vec3 c0 = ClosestPointOnLineSegment(v0, v1, point);
    Vec3 c1 = ClosestPointOnLineSegment(v1, v2, point);
    Vec3 c2 = ClosestPointOnLineSegment(v2, v0, point);

    // Find the closest of the three
    double dist0 = (c0 - point).dot(c0 - point);
    double dist1 = (c1 - point).dot(c1 - point);
    double dist2 = (c2 - point).dot(c2 - point);

    if (dist0 < dist1 && dist0 < dist2)
    {
        return c0;
    }

    if (dist1 < dist2)
    {
        return c1;
    }

    return c2;
}

