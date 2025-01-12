// Includes from project
#include "particle.hpp"


Spring::Spring(Particle* pParticle, const double restLength, const double stiffness, const double damping)
	: m_pParticle(pParticle), m_restLength(restLength), m_stiffness(stiffness), m_damping(damping)
{
}

Spring::~Spring()
{
}

/*
* Compute the force applied by the spring
* 
* @param position Position of the particle that instanciated the spring
* @param velocity Velocity of the particle that instanciated the spring
* @return Vec3 Force applied by the spring
*/
Vec3 Spring::computeForce(const Vec3& position, const Vec3& velocity)
{
	Vec3 force = m_pParticle->m_previousPosition - position;
	const double length = force.norm();
	force.normalize();

	// damping force is use to reduce the oscillation of the spring
	force *= -m_stiffness * (length - m_restLength) - m_damping * (m_pParticle->m_previousVelocity - velocity).dot(force);
	
	return force;
}


Particle::Particle()
{
}

Particle::~Particle()
{
}

Vec3 Particle::computeForces(const Vec3& gravity)
{
	// Gravity force
	Vec3 forces = gravity * m_mass;

	// Air friction
	forces -= m_previousVelocity * m_airFriction;

	// Object friction TODO

	// Spring forces
	for (const auto& spring : m_springs)
	{
		forces += spring->computeForce(m_previousPosition, m_previousVelocity);
	}

	return forces;
}

void Particle::computePFD(const Vec3& forces, const double dt)
{
	// Compute the acceleration
	m_acceleration = forces / m_mass;

	// Update position using the old velocity and acceleration. Verlet Integration (more accurate)
	m_position += m_previousVelocity * dt + m_acceleration * 0.5 * dt * dt;

	// Update velocity using the new acceleration
	m_velocity += m_acceleration * dt;
}

void Particle::update(const double dt)
{
	// Compute the forces
	const Vec3 gravity(0.0, -9.81, 0.0);
	const Vec3 forces = computeForces(gravity);

	// Compute the position, velocity and acceleration
	computePFD(forces, dt);

	// Handle collision with the ground
	if (m_position.y < 0.0)
	{
		m_position.y = 0.0;
		m_velocity.y = -m_velocity.y;
	}

	// Update the previous position and velocity
	m_previousPosition = m_position;
	m_previousVelocity = m_velocity;
}