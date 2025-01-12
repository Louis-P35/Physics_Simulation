// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData()
{
	m_testParticle.m_position = Vec3(0.0, 5.0, 0.0);

	// Initialize the last update time
	m_lastUpdateTime = std::chrono::steady_clock::now();
	
}

/*
* Update one frame of the simulation
* 
* @return bool True if the simulation was updated successfully, false otherwise
*/
bool ApplicationData::simulationUpdate()
{
	std::cout << "Simulation update: ";

	// Calculate the time elapsed since the last update
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - m_lastUpdateTime;
	m_lastUpdateTime = currentTime;  // Update the last update time to the current time

	// Convert deltaTime to seconds
	float elapsedTimeInSeconds = deltaTime.count();

	m_testParticle.update(static_cast<double>(elapsedTimeInSeconds));
	std::cout << m_testParticle.m_position.x << " " << m_testParticle.m_position.y << " " << m_testParticle.m_position.z << std::endl;

	// Outch need mutex...
	m_debugSphere3D.m_position = m_testParticle.m_position.toArray();

	return true;
}