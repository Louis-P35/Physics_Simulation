// Include from project
#include "cloth.hpp"


Cloth::Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position) :
	m_resX(resX), m_resY(resY), m_width(width), m_height(height), m_thickness(thickness), m_clothMass(clothMass), m_position(position)
{
	const int nbParticles = m_resX * m_resY;
	// Create particles
	for (int i = 0; i < m_resX; ++i)
	{
		std::vector<Particle> rowBottom;
		for (int j = 0; j < m_resY; ++j)
		{
			Vec3 pos = Vec3(m_position.x + static_cast<double>(i) * m_width / m_resX, m_position.y, m_position.z + static_cast<double>(j) * m_height / m_resY);
			double particleMass = clothMass / static_cast<double>(nbParticles);
			rowBottom.push_back(Particle(pos, particleMass));
		}

		m_particlesBottom.push_back(rowBottom);
	}

	// Create the springs
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			// Loop over the neighbors
			for (int ii = i - 1; ii <= i + 1; ++ii)
			{
				for (int jj = j - 1; jj <= j + 1; ++jj)
				{
					if ((ii == i && jj == j) || ii < 0 || ii >= m_resX || jj < 0 || jj >= m_resY)
					{
						continue;
					}
					const double distance = (m_particlesBottom[i][j].m_position - m_particlesBottom[ii][jj].m_position).norm();

					m_particlesBottom[i][j].m_springs.push_back(Spring(&m_particlesBottom[ii][jj], distance, 500.0, 0.0));
				}
			}

			// Debug
			if (j == (m_resY - 1) || j == 0 || i == 0 || i == (m_resX - 1))
			{
				m_particlesBottom[i][j].setFixed(true);
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
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			m_particlesBottom[i][j].update(dt);
		}
	}

	// Update the previous position and velocity
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			m_particlesBottom[i][j].m_previousPosition = m_particlesBottom[i][j].m_position;
			m_particlesBottom[i][j].m_previousVelocity = m_particlesBottom[i][j].m_velocity;
		}
	}
}
