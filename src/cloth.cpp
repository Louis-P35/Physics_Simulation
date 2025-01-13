// Include from project
#include "cloth.hpp"


Cloth::Cloth(int resX, int resY, double width, double height, double thickness, double particleMass, Vec3 position) :
	m_resX(resX), m_resY(resY), m_width(width), m_height(height), m_thickness(thickness), m_particleMass(particleMass), m_position(position)
{
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			Vec3 pos = Vec3(m_position.x + i * m_width / m_resX, m_position.y, m_position.z + j * m_height / m_resY);
			m_particles.push_back(Particle(pos, particleMass));
		}
	}
}