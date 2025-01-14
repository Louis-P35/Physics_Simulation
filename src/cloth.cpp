// Include from project
#include "cloth.hpp"


Cloth::Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position) :
	m_resX(resX), m_resY(resY), m_width(width), m_height(height), m_thickness(thickness), m_clothMass(clothMass), m_position(position)
{
	const int nbParticles = m_resX * m_resY;
	// Create particles
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			Vec3 pos = Vec3(m_position.x + static_cast<double>(i) * m_width / m_resX, m_position.y, m_position.z + static_cast<double>(j) * m_height / m_resY);
			double particleMass = clothMass / static_cast<double>(nbParticles);
			m_particles.push_back(Particle(pos, particleMass));
		}
	}

	// Create the springs
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			const int particleIndex = i * m_resY + j;

			const int particleIndexRight = (i + 1) * m_resY + j;
			const int particleIndexLeft = (i - 1) * m_resY + j;

			const int particleIndexUp = i * m_resY + j - 1;
			const int particleIndexDown = i * m_resY + j + 1;
			
			const int particleIndexDiag1 = (i + 1) * m_resY + j + 1;
			const int particleIndexDiag2 = (i + 1) * m_resY + j - 1;
			const int particleIndexDiag3 = (i - 1) * m_resY + j + 1;
			const int particleIndexDiag4 = (i - 1) * m_resY + j - 1;

			std::vector<int> neighboursIndexs = { 
				particleIndexRight,
				particleIndexLeft,
				particleIndexUp,
				particleIndexDown,
				particleIndexDiag1,
				particleIndexDiag2,
				particleIndexDiag3,
				particleIndexDiag4
			};

			for (int neighbourIndex : neighboursIndexs)
			{
				if (neighbourIndex >= 0 && neighbourIndex < m_particles.size())
				{
					const double distance = (m_particles[particleIndex].m_position - m_particles[neighbourIndex].m_position).norm();

					m_particles[particleIndex].m_springs.push_back(Spring(&m_particles[neighbourIndex], distance, 500.0, 0.0));
				}
			}

			// Debug
			if (j == (m_resY - 1) || j == 0 || i == 0 || i == (m_resX - 1))
			{
				m_particles[particleIndex].setFixed(true);
			}
		}
	}
}


/*
* Update the cloth by updating all the particles
* 
* @param dt Time step
* @return void
*/
void Cloth::update(double dt)
{
	// Clamp the time step to avoid huge time steps
	// This is a simple way to avoid instability in the simulation
	if (dt > 0.1)
	{
		dt = 0.1;
	}


	// Update the particles
	for (auto& particle : m_particles)
	{
		particle.update(dt);
	}

	// Update the previous position and velocity
	for (auto& particle : m_particles)
	{
		particle.m_previousPosition = particle.m_position;
		particle.m_previousVelocity = particle.m_velocity;
	}
}