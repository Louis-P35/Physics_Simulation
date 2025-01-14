#pragma once

// Includes from project
#include "particle.hpp"
#include "vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"

// Includes from STL
#include <vector>


class Cloth
{
public:
	int m_resX;
	int m_resY;
	double m_width;
	double m_height;
	double m_thickness;
	double m_clothMass;

	Vec3 m_position;

	std::vector<Particle> m_particles;

public:
	Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position);
	~Cloth() {};

	void update(double dt);
};