#pragma once

// Includes from project
#include "particle.hpp"
#include "vec3.hpp"

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
	double m_particleMass;

	Vec3 m_position;

	std::vector<Particle> m_particles;

public:
	Cloth(int resX, int resY, double width, double height, double thickness, double particleMass, Vec3 position);
	~Cloth() {};

	void update(const double dt);
};