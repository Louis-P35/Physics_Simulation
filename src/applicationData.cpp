// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData()
{
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
	// Create a cloth
	Vec3 position = Vec3(-4.0, 2.0, -4.0);
	if (!m_pCloth)
	{
		m_pCloth = std::make_shared<Cloth>(10, 10, 5.0, 5.0, 0.1, 300.0, position);
	}
	else
	{

	}

	// Initialize the last update time
	m_lastUpdateTime = std::chrono::steady_clock::now();

	// Start the simulation in a separate thread
	m_physicsWorker.start([&]() {
		// Perform physics updates
		simulationUpdate();

		// Emit a signal to update GUI if needed
		//QApplication::postEvent(&window, new QEvent(QEvent::UpdateRequest));
		});
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

	// Update the cloth's mesh
	m_pCloth->updateMesh();

	return true;
}


/*
* Reset the simulation
* This function is called by a slot in the MainWindow class
* 
* @return void
*/
void ApplicationData::resetSimulation()
{
	// Stop the physics simulation
	m_physicsWorker.stop();

	// Reset the and restart the simulation
	initSimulation();
}