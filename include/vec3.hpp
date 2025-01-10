#pragma once

// Includes from STD
#include <array>


/*
* Vec3 class
* 
* This class is used to represent a 3D vector
*/
class Vec3
{
public:
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;

	Vec3() {};
	Vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
	Vec3(const float x, const float y, const float z) : 
		x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z)) {}
	Vec3(const std::array<double, 3>& arr) : x(arr[0]), y(arr[1]), z(arr[2]) {}
	Vec3(const std::array<float, 3>& arr) : 
		x(static_cast<double>(arr[0])), y(static_cast<double>(arr[1])), z(static_cast<double>(arr[2])) {}

	Vec3 operator+(const Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	Vec3 operator+=(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;

		return Vec3(x, y, z);
	}

	Vec3 operator-(const Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	Vec3 operator*(const double scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}

	Vec3 operator/(const double scalar) const
	{
		return Vec3(x / scalar, y / scalar, z / scalar);
	}

	Vec3 cross(const Vec3& other) const
	{
		return Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	double dot(const Vec3& other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	double norm() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	Vec3 normalize() const
	{
		double len = norm();
		return Vec3(x / len, y / len, z / len);
	}

	Vec3 reflect(const Vec3& normal) const
	{
		return *this - normal * 2.0 * dot(normal);
	}

	std::array<float, 3> toArray() const
	{
		return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
	}
};