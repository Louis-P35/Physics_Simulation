// Includes from project
#include "clothFactory.hpp"

// Includes from 3rd party

// Includes from STL
#include <iostream>


/*
* Static method to create a cloth
* 
* @param resX Resolution of the cloth in the X direction
* @param resY Resolution of the cloth in the Y direction
* @param width Width of the cloth
* @param height Height of the cloth
* @param thickness Thickness of the cloth
* @param clothMass Mass of the cloth (mass of each particle is computed from this value)
* @param position Position of the cloth
* @param pOpenGl3DWidget OpenGl3DWidget to add the cloth to
* @param colliders List of colliders
* @return std::shared_ptr<Cloth> Pointer to the created cloth
*/
std::shared_ptr<Cloth> ClothFactory::createCloth(
	int resX,
	int resY,
	double width,
	double height,
	double thickness,
	double clothMass,
	Vec3 position,
	OpenGl3DWidget* pOpenGl3DWidget,
	std::vector<std::shared_ptr<Collider>>& colliders,
	std::shared_ptr<GridCollider> pGridCollider
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

	// Debug view of the particles
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
	pCloth->startWorker([pCloth, pColliders = &colliders, pGridCollider = pGridCollider]() {
		// Perform physics updates
		pCloth->updateSimulation(*pColliders, pGridCollider);
		});

	return pCloth;
}