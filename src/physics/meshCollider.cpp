// Includes from project
#include "meshCollider.hpp"

// Includes from STL



MeshCollider::MeshCollider(const Vec3& position, const Object3D& obj) : Collider(position)
{

}


bool MeshCollider::hasCollided(
	Vec3& collPosition,
	Vec3& collNormal,
	Vec3& bounceVect,
	const Vec3& p0,
	const Vec3& p1,
	const double partRadius
) const
{
	return false;
}


/*
* Detect if the input ray has collided with the mesh
* Use the Möller–Trumbore algorithm
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

    Vec3 h = ray.direction.cross(edge2);
    double a = edge1.dot(h);

    if (fabs(a) < EPSILON)
    {
        return false;  // Parallel case (no intersection)
    }

    double f = 1.0 / a;
    Vec3 s = ray.origin - v0;
    double u = f * s.dot(h);

    if (u < 0.0 || u > 1.0)
    {
        return false;  // Barycentric coordinate check
    }

    Vec3 q = s.cross(edge1);
    double v = f * ray.direction.dot(q);

    if (v < 0.0 || (u + v) > 1.0)
    {
        return false;  // Barycentric coordinate check
    }

    double t = f * edge2.dot(q);

    if (t > EPSILON) // Intersection valid if t is positive
    {  
        collPosition = ray.origin + ray.direction * t;
        return true;
    }

    return false;  // Intersection behind the ray origin
}
