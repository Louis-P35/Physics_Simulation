// Includes from project
#include "meshCollider.hpp"
#include "aabb.hpp"

// Includes from STL
#include <cmath>
#include <algorithm>
#include <iostream>



MeshCollider::MeshCollider(const Vec3& position, const Object3D& obj) : Collider(position)
{
	for (size_t i = 0; i < obj.m_faces.size(); i++)
	{
        Vec3 v0 = Vec3(obj.m_vertices[obj.m_faces[i][0]]);
        Vec3 v1 = Vec3(obj.m_vertices[obj.m_faces[i][3]]);
        Vec3 v2 = Vec3(obj.m_vertices[obj.m_faces[i][6]]);

		m_triangles.push_back({v0, v1, v2});
	}

	m_pRoot = std::make_shared<OctreeNode>(createAABB(m_triangles));
    createCollisionTree(m_pRoot, m_triangles, 0.035);
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
	const double partRadius,
    const AABB& aabb
) const
{
	if (!m_pRoot)
	{
		return false;
	}

	double length = (p1 - p0).norm();
	Vec3 particleDir = (p1 - p0).getNormalized();
	Vec3 particlePos = p0 - m_colliderPosition;

	double closestTriangleDist = std::numeric_limits<double>::max();
    bool hasCollided = false;
    Vec3 closestClosestPointOnT;
    Vec3 closestNormalOnT;

    AABB _aabb = AABB(aabb);
	_aabb.m_min -= m_colliderPosition;
	_aabb.m_max -= m_colliderPosition;
    std::vector<OctreeNode*> listCollidedAabb = m_pRoot->detectCollision(_aabb);
    for (OctreeNode* pNode : listCollidedAabb)
    {
		for (const auto& triangle : pNode->m_triangles)
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
					closestNormalOnT = (closestPointOnT - particlePos).getNormalized();
				}
			}
		}
	}

    if (hasCollided)
    {
        collPosition = closestClosestPointOnT + m_colliderPosition;
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


/*
* Create the AABB of the given triangles list
* 
* @param triangles The list of triangles
* @return AABB The AABB of the triangles
*/
AABB MeshCollider::createAABB(std::vector<std::array<Vec3, 3>>& triangles)
{
	Vec3 min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
	Vec3 max(std::numeric_limits<double>::min(), std::numeric_limits<double>::min(), std::numeric_limits<double>::min());

	for (const auto& triangle : triangles)
	{
		double minX = std::min({ triangle[0].x, triangle[1].x, triangle[2].x });
		double minY = std::min({ triangle[0].y, triangle[1].y, triangle[2].y });
		double minZ = std::min({ triangle[0].z, triangle[1].z, triangle[2].z });

		double maxX = std::max({ triangle[0].x, triangle[1].x, triangle[2].x });
		double maxY = std::max({ triangle[0].y, triangle[1].y, triangle[2].y });
		double maxZ = std::max({ triangle[0].z, triangle[1].z, triangle[2].z });

		min.x = std::min(min.x, minX);
		min.y = std::min(min.y, minY);
		min.z = std::min(min.z, minZ);

		max.x = std::max(max.x, maxX);
		max.y = std::max(max.y, maxY);
		max.z = std::max(max.z, maxZ);
	}

	return AABB(min, max);
}


/*
* Get the 8 sub AABBs from the parent AABB
* Divide the parent AABB into 8 sub AABBs
* 
* @param parentAabb The parent AABB
* @return std::vector<AABB> The list of 8 sub AABBs
*/
std::vector<AABB> MeshCollider::getSubAabbs(const AABB& parentAabb)
{
    // TOP
    // 0    1
    // 2	3

    // BOTTOM
    // 4    5
    // 6    7

	std::vector<AABB> subAabbs;

    double middleX = (parentAabb.m_min.x + parentAabb.m_max.x) / 2.0;
    double middleY = (parentAabb.m_min.y + parentAabb.m_max.y) / 2.0;
    double middleZ = (parentAabb.m_min.z + parentAabb.m_max.z) / 2.0;

    subAabbs.push_back(AABB(Vec3(parentAabb.m_min.x, middleY, middleZ), Vec3(middleX, parentAabb.m_max.y, parentAabb.m_max.z)));
    subAabbs.push_back(AABB(Vec3(middleX, middleY, middleZ), Vec3(parentAabb.m_max.x, parentAabb.m_max.y, parentAabb.m_max.z)));
    subAabbs.push_back(AABB(Vec3(parentAabb.m_min.x, parentAabb.m_min.y, middleZ), Vec3(middleX, middleY, parentAabb.m_max.z)));
    subAabbs.push_back(AABB(Vec3(middleX, parentAabb.m_min.y, middleZ), Vec3(parentAabb.m_max.x, middleY, parentAabb.m_max.z)));
    subAabbs.push_back(AABB(Vec3(parentAabb.m_min.x, middleY, parentAabb.m_min.z), Vec3(middleX, parentAabb.m_max.y, middleZ)));
    subAabbs.push_back(AABB(Vec3(middleX, middleY, parentAabb.m_min.z), Vec3(parentAabb.m_max.x, parentAabb.m_max.y, middleZ)));
    subAabbs.push_back(AABB(Vec3(parentAabb.m_min.x, parentAabb.m_min.y, parentAabb.m_min.z), Vec3(middleX, middleY, middleZ)));
    subAabbs.push_back(AABB(Vec3(middleX, parentAabb.m_min.y, parentAabb.m_min.z), Vec3(parentAabb.m_max.x, middleY, middleZ)));

    return subAabbs;
}


/*
* Create the collision tree
* Recursively climb down the tree to create the children
* When it reach a leaf, create it's bounding box
* Then climb back up to set the AABB of the currents nodes (fuse the AABB of the children)
*
* @param pRoot The current node of the tree
* @param iMin The minimum index in the X direction
* @param iMax The maximum index in the X direction
* @param jMin The minimum index in the Y direction
* @param jMax The maximum index in the Y direction
* @return std::shared_ptr<OctreeNode> The newly created node
*/
std::shared_ptr<OctreeNode> MeshCollider::createCollisionTree(
    std::shared_ptr<OctreeNode> pRoot,
    std::vector<std::array<Vec3, 3>>& triangles,
    const double particleRadius
)
{
    if (!pRoot || triangles.size() == 0)
    {
        return nullptr;
    }

    // At this point, pRoot is not yet added to the tree

    // If is leaf
    if (triangles.size() == 1)
    {
        // Set the AABB of the leaf to be the AABB of the triangle + the particle radius
        pRoot->m_aabb.setAabb(createAABB(triangles));
		pRoot->m_aabb.m_min -= Vec3(particleRadius, particleRadius, particleRadius);
		pRoot->m_aabb.m_max += Vec3(particleRadius, particleRadius, particleRadius);

		pRoot->m_triangles.push_back(triangles[0]);

        // Return the leaf
        return pRoot;
    }

    // Going down the tree, we create the children of the current node
    // At this point pRoot can not be a leaf

	// Split the AABB of the current node into 8 sub AABBs
    std::vector<AABB> subAabbs = getSubAabbs(pRoot->m_aabb);

	// Split the triangles into the children
    std::vector<std::array<Vec3, 3>> childrenTriangles[8];
	
	for (const auto& triangle : triangles)
	{
		// Find the closest sub AABB
		double minDist = std::numeric_limits<double>::max();
		int index = -1;
		for (int i = 0; i < 8; ++i)
		{
			Vec3 center = (subAabbs[i].m_min + subAabbs[i].m_max) / 2.0;
			double dist = (center - (triangle[0] + triangle[1] + triangle[2]) / 3.0).norm();
			if (dist < minDist)
			{
				minDist = dist;
                index = i;
			}
		}
		
        if (index >= 0 && index < 8)
		{
            childrenTriangles[index].push_back(triangle);
		}
	}

	// Create the children
	for (int i = 0; i < 8; ++i)
	{
		if (childrenTriangles[i].empty())
		{
			continue;
		}

        if (childrenTriangles[i].size() == triangles.size())
        {
			// All the triangles are in the same child
			// We can't split the triangles anymore in some cases...
			// So we create a leaf
			std::shared_ptr<OctreeNode> pChild = std::make_shared<OctreeNode>(createAABB(childrenTriangles[i]));
            pChild->m_aabb.m_min -= Vec3(particleRadius, particleRadius, particleRadius);
            pChild->m_aabb.m_max += Vec3(particleRadius, particleRadius, particleRadius);
			pRoot->addChildren(pChild);
			pChild->m_triangles.insert(pChild->m_triangles.end(), childrenTriangles[i].begin(), childrenTriangles[i].end());

			continue;
        }

		std::shared_ptr<OctreeNode> pChild = std::make_shared<OctreeNode>(subAabbs[i]);
		pRoot->addChildren(createCollisionTree(pChild, childrenTriangles[i], particleRadius));
	}

    // Climbing back up the tree, we set the AABB of the current node
    // Fuse the AABB of the children
    std::vector<AABB> aabbs;
    for (const auto& pChild : pRoot->m_pChildren)
    {
		if (pChild)
		{
            aabbs.push_back(pChild->m_aabb);
		}
    }
    pRoot->m_aabb.setAabb(aabbs);

    return pRoot;
}
