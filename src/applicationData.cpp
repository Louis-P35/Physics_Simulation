// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData()
{

}


/*
* Initialize the simulation
* 
* @return bool True if the simulation was initialized successfully, false otherwise
*/
bool ApplicationData::initSimulation()
{
	static double high = 1.0;
	if (!m_pOpenGl3DWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return false;
	}

	// Create a cloth
	Vec3 position = Vec3(-4.0, /*2.0*/high, -4.0);
	std::shared_ptr<Cloth> pCloth = std::make_shared<Cloth>(10, 10, 5.0, 5.0, 0.1, 300.0, position);
	m_pCloths.push_back(pCloth);
	high += 0.5;
	/*if (!m_pCloth)
	{
		m_pCloth = std::make_shared<Cloth>(10, 10, 5.0, 5.0, 0.1, 300.0, position);
	}
	else
	{
		std::cerr << "Error: Cloth already created" << std::endl;
		return false;
	}*/

	// Add the mesh of the cloth to the rendering widget
	//if (auto clothObject = std::dynamic_pointer_cast<Object3D>(pCloth))
	//{
	pCloth->m_pRenderingInstance = m_pOpenGl3DWidget->addObject(pCloth->m_object3D);//*clothObject);
	pCloth->m_pRenderingInstance->m_isStatic = false;
	//}

	// Start the simulation in a separate thread
	// Capture a copy on the pointer instead of a reference to avoid a dangling pointer
	pCloth->startWorker([pCloth]() {
		// Perform physics updates
		pCloth->updateSimulation();

		// Emit a signal to update GUI if needed
		//QApplication::postEvent(&window, new QEvent(QEvent::UpdateRequest));
		});

	/*for (int i = 0; i < m_pCloth->m_resX; ++i)
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
	}*/
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
	//m_pCloth->stopWorker();

	//m_pCloth = nullptr;

	//m_pOpenGl3DWidget->removeAllObjects();


	// Reset the and restart the simulation
	int res = initSimulation();

	return res;
}