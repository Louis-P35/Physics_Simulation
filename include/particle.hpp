#pragma once

// Includes from project
#include "vec3.hpp"

// Includes from STL
#include <vector>
#include <memory>

class Particle;


class Spring
{
private:
	Particle* m_pParticle;
	double m_restLength;
	double m_stiffness;
	double m_damping;

public:
	Spring(Particle* pParticle, const double restLength, const double stiffness, const double damping);
	~Spring();

	Vec3 computeForce(const Vec3& position, const Vec3& velocity);
};


class Particle
{
public:
	Vec3 m_position;
	Vec3 m_previousPosition;
	Vec3 m_velocity;
	Vec3 m_previousVelocity;
	Vec3 m_acceleration;

	double m_mass = 1.0;
	double m_airFriction = 1.0;
	double m_objectFriction = 1.0;

	std::vector<std::unique_ptr<Spring>> m_springs;

public:
	Particle();
	~Particle();

private:
	Vec3 computeForces(const Vec3& gravity);
	void computePFD(const Vec3& forces, const double dt);
};