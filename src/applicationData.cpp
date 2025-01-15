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

	// Initialize the last update time
	m_lastUpdateTime = std::chrono::steady_clock::now();
	
}


/*
* Initialize the simulation
* 
* @return void
*/
void ApplicationData::initSimulation()
{
	Vec3 position = Vec3(-4.0, 2.0, -4.0);
	m_pCloth = std::make_shared<Cloth>(30, 30, 5.0, 5.0, 0.0, 300.0, position);

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

	// Skip the first update to avoid a huge time step
	if (firstUpdate)
	{
		firstUpdate = false;
		return true;
	}

	// Convert deltaTime to seconds
	float elapsedTimeInSeconds = deltaTime.count();

	// Update the simulation
	m_pCloth->update(elapsedTimeInSeconds);

	return true;
}