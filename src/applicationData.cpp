// Includes from project
#include "applicationData.hpp"
#include "../src/physics/sphereCollider.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData() : m_pOpenGl3DWidget(nullptr)
{
	m_colliders.push_back(std::make_shared<SphereCollider>(Vec3(-2.5, 1.5, -2.5), 1.0));
}


ApplicationData::~ApplicationData()
{
	std::cout << "STOP WORKERS" << std::endl;
	// Stop the physics simulation for all the cloths
	for (auto& pCloth : m_pCloths)
	{
		if (pCloth)
		{
			pCloth->stopWorker();
		}
	}
}


bool ApplicationData::initAfterOpenGl(OpenGl3DWidget* pGl3dWidget)
{
	//Object3D test;
	//m_monkey3D.loadFromObjFile("../models/test2/", "testCube.obj");

	//m_monkey3D.loadFromObjFile("../models/Susanne/", "suzanne.obj");
	//m_monkey3D.loadFromObjFile("../models/sphere/", "sphere.obj");

	//std::shared_ptr<Object3D> pCube = std::make_shared<Object3D>();
	//m_cube3D.loadFromObjFile("../models/cube/", "cube.obj");
	//m_pCube3DRenderer = pGl3dWidget->addObject(m_cube3D);
	//m_pCube3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//m_pCube3DRenderer->m_pPosRotScale->m_scale = { 0.3f, 0.3f, 0.3f };

	std::shared_ptr<Object3D> pCube = std::make_shared<Object3D>();
	m_cube3D.loadFromObjFile("../models/sphere_highRes/", "untitled.obj");
	//m_cube3D.loadFromObjFile("../models/cube/", "cube.obj");
	m_pCube3DRenderer = pGl3dWidget->addObject(m_cube3D);
	m_pCube3DRenderer->m_pPosRotScale->m_position = { -2.5, 1.5, -2.5 };
	m_pCube3DRenderer->m_pPosRotScale->m_scale = { 1.0f, 1.0f, 1.0f };

	// Ground
	m_ground3D.loadFromObjFile("../models/ground_2/", "ground.obj");
	m_pGround3DRenderer = pGl3dWidget->addObject(m_ground3D);
	m_pGround3DRenderer->m_pPosRotScale->m_position = { 0.0f, 0.0f, 0.0f };
	m_pGround3DRenderer->m_pPosRotScale->m_scale = { 1.0f, 1.0f, 1.0f };

	// Debug sphere
	//m_debugSphere3D.loadFromObjFile("../models/sphere/", "sphere.obj");
	//m_pDebugSphere3DRenderer = pGl3dWidget->addObject(m_debugSphere3D);
	//m_pDebugSphere3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//m_pDebugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.1f, 0.1f, 0.1f };

	return true;
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
	std::shared_ptr<Cloth> pCloth = std::make_shared<Cloth>(40, 40, 5.0, 5.0, 0.025, 300.0, position);
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