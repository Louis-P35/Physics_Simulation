#pragma once

// Includes from project
#include "vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"

// Includes from STL
#include <vector>
#include <memory>

class Particle;

/*
* Spring class
* 
* This class is used to represent a spring in a physics simulation
* 
* The spring is used to link two particles together
*/
class Spring
{
private:
	Particle* m_pParticle;
	double m_restLength;
	double m_stiffness;
	double m_damping;

	//Object3D m_debugSphere;

public:
	Spring(Particle* pParticle, const double restLength, const double stiffness, const double damping);
	~Spring();

	Vec3 computeForce(const Vec3& position, const Vec3& velocity) const;
};


/*
* Particle class
* 
* This class is used to represent a particle in a physics simulation
* 
* The particle can be linked to other particles with springs
*/
class Particle
{
public:
	Vec3 m_position;
	Vec3 m_previousPosition;
	Vec3 m_velocity;
	Vec3 m_previousVelocity;
	Vec3 m_acceleration;

	double m_mass = 1.0;
	double m_airFriction = 0.2;
	double m_objectFriction = 1.0;

	std::vector<Spring> m_springs;

public:
	Particle(Vec3 position, double mass);
	~Particle();

	void update(const double dt);

private:
	Vec3 computeForces(const Vec3& gravity);
	void computePFD(const Vec3& forces, const double dt);
};