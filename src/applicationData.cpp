// Includes from project
#include "applicationData.hpp"
#include "../src/physics/sphereCollider.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData() : m_pOpenGl3DWidget(nullptr)
{
	m_colliders.push_back(std::make_shared<SphereCollider>(Vec3(-1.5, 1.0, -1.5), 1.0));
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
	Vec3 position = Vec3(-4.0, 2.5, -4.0);
	std::shared_ptr<Cloth> pCloth = std::make_shared<Cloth>(30, 30, 5.0, 5.0, 0.1, 300.0, position);
	m_pCloths.push_back(pCloth);

	// Add the mesh of the cloth to the rendering widget
	pCloth->m_pRenderingInstance = m_pOpenGl3DWidget->addObject(pCloth->m_object3D);
	pCloth->m_pRenderingInstance->m_isStatic = false;

	/*for (int i = 0; i < pCloth->m_resX; ++i)
	{
		for (int j = 0; j < pCloth->m_resY; ++j)
		{
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D);
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth->m_particlesBottom[i][j].m_position.toArray();
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };

			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D);
			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth->m_particlesTop[i][j].m_position.toArray();
			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };
		}
	}*/

	// Start the simulation in a separate thread
	// Capture a copy on the pointer instead of a reference to avoid a dangling pointer
	pCloth->startWorker([pCloth, pColliders = &m_colliders]() {
		// Perform physics updates
		pCloth->updateSimulation(*pColliders);

		// Emit a signal to update GUI if needed
		//QApplication::postEvent(&window, new QEvent(QEvent::UpdateRequest));
		});
}


/*
* Reset the simulation
* This function is called by a slot in the MainWindow class
* 
* @return void
*/
bool ApplicationData::resetSimulation()
{
	// Stop the physics simulation for all the cloths
	for (auto& pCloth : m_pCloths)
	{
		pCloth->stopWorker();
	}

	// Remove all the cloths from the rendering widget
	for (auto& pCloth : m_pCloths)
	{
		m_pOpenGl3DWidget->removeObject(pCloth->m_pRenderingInstance);
	}

	// Clear the list of cloths
	m_pCloths.clear();

	// Reset the and restart the simulation
	int res = initSimulation();

	return res;
}