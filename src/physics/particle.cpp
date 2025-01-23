// Includes from project
#include "particle.hpp"

// Includes from STL
#include <iostream>


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
Vec3 Spring::computeForce(const Vec3& position, const Vec3& velocity) const
{
	Vec3 force = m_pParticle->m_previousPosition - position;
	const double length = force.norm();
	force.normalize();

	// damping force is use to reduce the oscillation of the spring
	force *= m_stiffness * (length - m_restLength) - m_damping * (m_pParticle->m_previousVelocity - velocity).dot(force);
	
	return force;
}


Particle::Particle(Vec3 position, double mass) : m_position(position), m_previousPosition(position), m_mass(mass)
{
	m_velocity = Vec3(0.0, 0.0, 0.0);
	m_previousVelocity = Vec3(0.0, 0.0, 0.0);
	m_acceleration = Vec3(0.0, 0.0, 0.0);
}

Particle::~Particle()
{
}

Vec3 Particle::computeForces(const Vec3& gravity)
{
	// Gravity force
	Vec3 forces = gravity * m_mass;

	// Air friction
	double velNorm = m_previousVelocity.norm();
	forces -= m_previousVelocity.getNormalized() * m_airFriction * velNorm * velNorm;

	// Object friction TODO

	// Spring forces
	for (const auto& spring : m_springs)
	{
		forces += spring.computeForce(m_previousPosition, m_previousVelocity);
	}

	return forces;
}

void Particle::computePFD(const Vec3& forces, const double dt)
{
	// Compute the acceleration
	m_acceleration = forces / m_mass;

	// Update velocity using the new acceleration
	m_velocity += m_acceleration * dt;

	// Update position using the old velocity and acceleration. Verlet Integration (more accurate)
	m_position += m_velocity * dt;
}

void Particle::update(const double dt, const std::vector<std::shared_ptr<Collider>>& colliders)
{
	// Do not update the particle if it is fixed
	if (isFixed)
	{
		return;
	}

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
		if (m_groundFriction > 0.0)
		{
			m_velocity *= 1.0 / m_groundFriction;
		}
	}

	// Handle collision with the colliders
	for (const auto& pCollider : colliders)
	{
		if (!pCollider)
		{
			continue;
		}

		Vec3 collPosition;
		Vec3 collNormal;
		Vec3 bounceVect;
		if (pCollider->hasCollided(collPosition, collNormal, bounceVect, m_previousPosition, m_position))
		{
			// Compute the new position
			m_position = collPosition;
			// Compute the new velocity
			m_velocity = bounceVect * m_velocity.norm();
			if (m_objectFriction > 0.0)
			{
				m_velocity *= 1.0 / m_objectFriction;
			}
		}
	}

	// Update the debug sphere position
	if (m_debugSphere3DRenderer)
	{
		// TODO: Outch, need mutex...
		m_debugSphere3DRenderer->m_pPosRotScale->m_position = m_position.toArray();
	}
}