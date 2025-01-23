// Includes from project
#include "objectsFactory.hpp"
#include "../src/physics/sphereCollider.hpp"

// Includes from 3rd party

// Includes from STL
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;


void ObjectsFactory::load3dObject(
	Object3D& object3d,
	std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
	std::shared_ptr<Collider>& pCollider,
	OpenGl3DWidget* pGl3dWidget,
	const std::string& folderName,
	const std::string& fileName,
	Vec3 pos,
	Vec3 scale
)
{
	const fs::path texturePath = g_pReourcesPath / fs::path(folderName) / fileName;

	// Load the object
	object3d.loadFromObjFile("../models/sphere_highRes/", "untitled.obj");
	
	// Add the object to the rendering list
	pRenderingInstance = pGl3dWidget->addObject(object3d);
	pRenderingInstance->m_pPosRotScale->m_position = pos.toArray();
	pRenderingInstance->m_pPosRotScale->m_scale = scale.toArray();

	// Create the collider (Sphere TODO change that)
	pCollider = std::make_shared<SphereCollider>(pos, 1.0);
}