#pragma once

// Includes from project
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/physics/collider.hpp"

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
	double m_airFriction = 2.0;
	double m_objectFriction = 1.25;

	std::vector<Spring> m_springs;

	std::shared_ptr<ObjectRenderingInstance> m_debugSphere3DRenderer;

private:
	bool isFixed = false;

public:
	Particle(Vec3 position, double mass);
	~Particle();

	void update(const double dt, const std::vector<std::shared_ptr<Collider>>& colliders);
	void setFixed(const bool fixState) { isFixed = fixState; };

private:
	Vec3 computeForces(const Vec3& gravity);
	void computePFD(const Vec3& forces, const double dt);
};