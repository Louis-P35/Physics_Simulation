#pragma once
#include "../src/math/Vec3.hpp"


bool approximatelyEqual(double a, double b, double epsilon = 1e-5);

void assertVec3Near(const Vec3& v1, const Vec3& v2, double epsilon = 1e-5);