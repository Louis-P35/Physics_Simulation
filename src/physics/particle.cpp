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
	m_externalForces = Vec3(0.0, 0.0, 0.0);

	// Assign a unique ID to the particle
	static size_t id = 0;
	m_id = id;
	id++;
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

	forces += m_externalForces;
	m_externalForces = Vec3(0.0, 0.0, 0.0);

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
	double normVel = m_velocity.norm();
	if (normVel > 5.0)
	{
		m_velocity *= 5.0 / normVel;
	}

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

	// Update the debug sphere position
	if (m_debugSphere3DRenderer)
	{
		// TODO: Outch, need mutex...
		m_debugSphere3DRenderer->m_pPosRotScale->m_position = m_position.toArray();
	}
}


/*
* Bounce the particle on a collision
* 
* @param normal Normal of the collision
* @param restitution Coefficient of restitution
* @return void
*/
void Particle::bounceOnCollision(const Vec3& normal, const double restitution)
{
	m_velocity = m_velocity.getReflected(normal);
	m_velocity *= restitution;
}


bool Particle::detectCollision(Particle& p1, Particle& p2)
{
	// TODO: Use aabb first

	const double distance = (p1.m_previousPosition - p2.m_previousPosition).norm();

	// Assume radius is the same for all particles
	const double radius = p1.m_pAabb->m_halfSize;

	if (distance < (2.0 * radius))
	{
		Vec3 dir = (p1.m_previousPosition - p2.m_previousPosition).getNormalized();
		double displace = ((2.0 * radius) - distance) / 2.0; // Displace both particles by half the distance

		// Replace the particles
		p1.m_position += dir * displace;
		p2.m_position -= dir * displace;

		// Bounce the velocity
		p1.m_velocity = p1.m_velocity.getReflected(dir);
		p1.m_velocity *= 0.3;
		p2.m_velocity = p2.m_velocity.getReflected(dir);
		p2.m_velocity *= 0.3;

		return true;
	}

	return false;
}


void Particle::resolveElasticCollision(Particle& p1, Particle& p2, const double restitution)
{

}