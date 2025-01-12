// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData()
{
	m_testParticle.m_position = Vec3(0.0, 5.0, 0.0);
	
}

bool ApplicationData::simulationUpdate()
{
	std::cout << "Simulation update: ";
	m_testParticle.update(0.1);
	std::cout << m_testParticle.m_position.x << " " << m_testParticle.m_position.y << " " << m_testParticle.m_position.z << std::endl;

	return true;
}