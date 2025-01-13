// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData()
{
	/*m_testParticle.m_position = Vec3(0.0, 2.0, 0.0);
	m_testParticle.m_previousPosition = m_testParticle.m_position;
	m_testParticle2.m_position = Vec3(-1.0, 2.0, 0.0);
	m_testParticle2.m_previousPosition = m_testParticle2.m_position;
	double lenght = (m_testParticle2.m_position - m_testParticle.m_position).norm();
	m_testParticle.m_springs.push_back(Spring(&m_testParticle2, lenght, 100.0, 0.0));*/

	Vec3 position = Vec3(0.0, 2.0, 0.0);
	m_pCloth = std::make_shared<Cloth>(10, 10, 5.0, 5.0, 0.0, 1.0, position);

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
	static bool firstUpdate = true;

	// Calculate the time elapsed since the last update
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - m_lastUpdateTime;
	m_lastUpdateTime = currentTime;

	if (firstUpdate)
	{
		firstUpdate = false;
		return true;
	}

	// Convert deltaTime to seconds
	float elapsedTimeInSeconds = deltaTime.count();

	//m_testParticle.update(static_cast<double>(elapsedTimeInSeconds));
	
	// Outch, need mutex...
	//m_debugSphere3D.m_position = m_testParticle.m_position.toArray();

	return true;
}