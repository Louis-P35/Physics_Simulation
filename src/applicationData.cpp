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
* @return bool True if the simulation was initialized successfully, false otherwise
*/
bool ApplicationData::initSimulation()
{
	if (!m_pOpenGl3DWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return false;
	}

	// Create a cloth
	Vec3 position = Vec3(-4.0, 2.0, -4.0);
	if (!m_pCloth)
	{
		m_pCloth = std::make_shared<Cloth>(10, 10, 5.0, 5.0, 0.1, 300.0, position);
	}
	else
	{
		std::cerr << "Error: Cloth already created" << std::endl;
		return false;
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

	// Add the mesh of the cloth to the rendering widget
	if (auto clothObject = std::dynamic_pointer_cast<Object3D>(m_pCloth))
	{
		m_pCloth->m_pRenderingInstance = m_pOpenGl3DWidget->addObject(*clothObject);
		m_pCloth->m_pRenderingInstance->m_isStatic = false;
	}

	for (int i = 0; i < m_pCloth->m_resX; ++i)
	{
		for (int j = 0; j < m_pCloth->m_resY; ++j)
		{
			m_pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D);
			m_pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = m_pCloth->m_particlesBottom[i][j].m_position.toArray();
			m_pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };

			m_pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D);
			m_pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = m_pCloth->m_particlesTop[i][j].m_position.toArray();
			m_pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };
		}
	}
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
bool ApplicationData::resetSimulation()
{
	// Stop the physics simulation
	m_physicsWorker.stop();

	m_pCloth = nullptr;

	// Reset the and restart the simulation
	int res = initSimulation();

	return res;
}