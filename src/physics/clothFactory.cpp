// Includes from project
#include "clothFactory.hpp"

// Includes from 3rd party

// Includes from STL
#include <iostream>


std::shared_ptr<Cloth> ClothFactory::createCloth(
	int resX,
	int resY,
	double width,
	double height,
	double thickness,
	double clothMass,
	Vec3 position,
	OpenGl3DWidget* pOpenGl3DWidget,
	std::vector<std::shared_ptr<Collider>>& colliders
)
{
	if (!pOpenGl3DWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return nullptr;
	}

	// Create the cloth
	std::shared_ptr<Cloth> pCloth = std::make_shared<Cloth>(resX, resY, width, height, thickness, clothMass, position);

	// Add the mesh of the cloth to the rendering widget
	pCloth->m_pRenderingInstance = pOpenGl3DWidget->addObject(pCloth->m_object3D);
	pCloth->m_pRenderingInstance->m_isStatic = false;

	/*for (int i = 0; i < pCloth->m_resX; ++i)
	{
		for (int j = 0; j < pCloth->m_resY; ++j)
		{
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer = pOpenGl3DWidget->addObject(m_debugSphere3D);
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth->m_particlesBottom[i][j].m_position.toArray();
			pCloth->m_particlesBottom[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };

			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer = pOpenGl3DWidget->addObject(m_debugSphere3D);
			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth->m_particlesTop[i][j].m_position.toArray();
			pCloth->m_particlesTop[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.05f, 0.05f, 0.05f };
		}
	}*/

	// Start the simulation in a separate thread
	// Capture a copy of the pointer instead of a reference to avoid a dangling pointer
	pCloth->startWorker([pCloth, pColliders = &colliders]() {
		// Perform physics updates
		pCloth->updateSimulation(*pColliders);
		});

	return pCloth;
}