// Includes from project
#include "applicationData.hpp"
#include "../src/physics/sphereCollider.hpp"
#include "clothFactory.hpp"
#include "objectsFactory.hpp"

// Includes from STL
#include <iostream>



ApplicationData::ApplicationData() : m_pOpenGl3DWidget(nullptr)
{
	
}


ApplicationData::~ApplicationData()
{
	// Stop the physics simulation for all the cloths
	m_pCloths.stopSimulation();

}


bool ApplicationData::initAfterOpenGl(OpenGl3DWidget* pGl3dWidget)
{
	if (!pGl3dWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return false;
	}

	//Object3D test;
	//m_monkey3D.loadFromObjFile("../models/test2/", "testCube.obj");

	//m_monkey3D.loadFromObjFile("../models/Susanne/", "suzanne.obj");
	//m_monkey3D.loadFromObjFile("../models/sphere/", "sphere.obj");

	//std::shared_ptr<Object3D> pCube = std::make_shared<Object3D>();
	//m_cube3D.loadFromObjFile("../models/cube/", "cube.obj");
	//m_pCube3DRenderer = pGl3dWidget->addObject(m_cube3D);
	//m_pCube3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//m_pCube3DRenderer->m_pPosRotScale->m_scale = { 0.3f, 0.3f, 0.3f };

	std::shared_ptr<Collider> pCollider = nullptr;
	ObjectsFactory::createSphere(
		m_cube3D,
		m_pCube3DRenderer,
		pCollider,
		pGl3dWidget,
		Vec3(-3.5, 3.0, -2.5),
		1.0
	);
	m_colliders.push_back(pCollider);

	Object3D sphere3D;
	std::shared_ptr<ObjectRenderingInstance> pSphere3DRenderer;
	std::shared_ptr<Collider> pCollider2 = nullptr;
	ObjectsFactory::createSphere(
		sphere3D,
		pSphere3DRenderer,
		pCollider2,
		pGl3dWidget,
		Vec3(-1.0, 3.0, -2.5),
		1.0
	);
	m_colliders.push_back(pCollider2);

	Object3D sphere3D2;
	std::shared_ptr<ObjectRenderingInstance> pSphere3DRenderer2;
	std::shared_ptr<Collider> pCollider3 = nullptr;
	ObjectsFactory::createSphere(
		sphere3D2,
		pSphere3DRenderer2,
		pCollider3,
		pGl3dWidget,
		Vec3(-2.20, 1.0, -2.5),
		1.0
	);
	m_colliders.push_back(pCollider3);
	

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

	return initSimulation();
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

	m_pGridCollider = std::make_shared<GridCollider>(0.2);

	const int res = 20;
	// Create a cloth
	Vec3 position = Vec3(-4.0, 4.3, -4.0);
	std::shared_ptr<Cloth> pCloth = ClothFactory::createCloth(
		res, res, 
		3.0, 3.0, 
		0.025, 300.0, 
		position, 
		m_pOpenGl3DWidget, 
		m_colliders,
		m_pGridCollider,
		m_pCloths
	);
	if (pCloth)
	{
		m_pCloths.addCloth(pCloth);
	}

	Vec3 position2 = Vec3(-3.0, 4.3, -4.0);
	std::shared_ptr<Cloth> pCloth2 = ClothFactory::createCloth(res, res, 3.0, 3.0, 0.025, 300.0, position2, m_pOpenGl3DWidget, m_colliders, m_pGridCollider, m_pCloths);
	if (pCloth2)
	{
		m_pCloths.addCloth(pCloth2);
	}

	Vec3 position3 = Vec3(-3.5, 4.5, -4.0);
	std::shared_ptr<Cloth> pCloth3 = ClothFactory::createCloth(res, res, 3.0, 3.0, 0.025, 300.0, position3, m_pOpenGl3DWidget, m_colliders, m_pGridCollider, m_pCloths);
	if (pCloth3)
	{
		m_pCloths.addCloth(pCloth3);
	}
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
	m_pCloths.stopSimulation();

	// No need of mutex from here because threads are stopped
	
	// Remove all the cloths from the rendering widget
	for (auto& [uid, pCloth] : m_pCloths.m_pClothsMap)
	{
		if (pCloth)
		{
			m_pOpenGl3DWidget->removeObject(pCloth->m_pRenderingInstance);
		}
	}

	// Clear the list of cloths
	m_pCloths.clearClothes();

	// Reset the barrier threshold for threads synchronization
	ClothFactory::s_barrier.setThreshold(0);

	// Reset the and restart the simulation
	int res = initSimulation();

	return res;
}