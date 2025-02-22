// Includes from project
#include "clothFactory.hpp"

// Includes from STL
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


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
	double colliderRadius,
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

	// Texture folders
	static std::vector<fs::path> s_textureFolders;
	static size_t s_currentTextureFolderIndex = 0;
	if (s_textureFolders.size() == 0)
	{
		const fs::path texturePath = fs::path("../models/fabrics_textures/");// / m_textureFolderName;
		try
		{
			for (const auto& entry : fs::directory_iterator(texturePath))
			{
				if (entry.is_directory())
				{
					s_textureFolders.push_back(entry.path());
				}
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << '\n';
		}
	}

	// Create the cloth
	std::shared_ptr<Cloth> pCloth = std::make_shared<Cloth>(
		resX, resY, 
		width, height, 
		colliderRadius, 
		thickness, 
		clothMass, 
		position,
		s_textureFolders.size() > 0 ? s_textureFolders[s_currentTextureFolderIndex].string() : ""
	);

	s_currentTextureFolderIndex++;
	if (s_currentTextureFolderIndex >= s_textureFolders.size())
	{
		s_currentTextureFolderIndex = 0;
	}

	// Add the mesh of the cloth to the rendering widget
	pCloth->m_pRenderingInstance = pOpenGl3DWidget->addObject(pCloth->m_object3D);
	pCloth->m_pRenderingInstance->m_isStatic = false;

	return pCloth;
}