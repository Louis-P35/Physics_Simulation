#include <gtest/gtest.h>
#include "../src/math/Vec3.hpp"
#include "../src/physics/meshCollider.hpp"
#include "utils.hpp"


TEST(RayTriangleIntersectionTest, RayHitsTriangle)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.25, 0.25, -1.0);
    ray.direction = Vec3(0.0, 0.0, 1.0).getNormalized();

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_TRUE(result);
    assertVec3Near(collisionPoint, Vec3(0.25, 0.25, 0.0));
    assertVec3Near(collisionNormal, Vec3(0.0, 0.0, 1.0)); // Expected normal
}

TEST(RayTriangleIntersectionTest, RayMissesTriangleDueToDirection)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.25, 0.25, -1.0);
    ray.direction = Vec3(1.0, 1.0, 0.0).getNormalized(); // Wrong direction

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_FALSE(result);
}

TEST(RayTriangleIntersectionTest, RayMissesOutsideTriangle)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(1.5, 1.5, -1.0); // Outside triangle
    ray.direction = Vec3(0.0, 0.0, 1.0).getNormalized();

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_FALSE(result);
}

TEST(RayTriangleIntersectionTest, RayHitsTriangleEdge)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.5, 0.0, -1.0); // Directly above edge v0-v1
    ray.direction = Vec3(0.0, 0.0, 1.0).getNormalized();

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_TRUE(result);
    assertVec3Near(collisionPoint, Vec3(0.5, 0.0, 0.0));
    assertVec3Near(collisionNormal, Vec3(0.0, 0.0, 1.0));
}

TEST(RayTriangleIntersectionTest, RayHitsTriangleVertex)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.0, 0.0, -1.0); // Directly above v0
    ray.direction = Vec3(0.0, 0.0, 1.0).getNormalized();

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_TRUE(result);
    assertVec3Near(collisionPoint, Vec3(0.0, 0.0, 0.0));
    assertVec3Near(collisionNormal, Vec3(0.0, 0.0, 1.0));
}

TEST(RayTriangleIntersectionTest, RayHitsBackface)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.25, 0.25, 1.0); // Starts behind the triangle
    ray.direction = Vec3(0.0, 0.0, -1.0).getNormalized();

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_TRUE(result);
    assertVec3Near(collisionPoint, Vec3(0.25, 0.25, 0.0));
    assertVec3Near(collisionNormal, Vec3(0.0, 0.0, 1.0)); // Normal should still be facing forward
}

TEST(RayTriangleIntersectionTest, RayOriginInsideTriangle)
{
    Vec3 v0(0.0, 0.0, 0.0);
    Vec3 v1(1.0, 0.0, 0.0);
    Vec3 v2(0.0, 1.0, 0.0);

    Ray ray;
    ray.origin = Vec3(0.25, 0.25, 0.0); // Inside the triangle
    ray.direction = Vec3(0.0, 0.0, 1.0).getNormalized(); // Going upwards

    Vec3 collisionPoint, collisionNormal;
    bool result = MeshCollider::rayTriangleIntersection(ray, v0, v1, v2, collisionPoint, collisionNormal);

    EXPECT_FALSE(result); // No intersection because ray starts on the triangle
}

