#include <gtest/gtest.h>
#include "utils.hpp"
#include <cmath>


bool approximatelyEqual(double a, double b, double epsilon)
{
    return std::fabs(a - b) < epsilon;
}

void assertVec3Near(const Vec3& v1, const Vec3& v2, double epsilon)
{
    EXPECT_NEAR(v1.x, v2.x, epsilon);
    EXPECT_NEAR(v1.y, v2.y, epsilon);
    EXPECT_NEAR(v1.z, v2.z, epsilon);
}