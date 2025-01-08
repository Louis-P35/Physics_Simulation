#pragma once


/*
* Vec3 class
* 
* This class is used to represent a 3D vector
*/
class Vec3
{
public:
	double x;
	double y;
	double z;

	Vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
};