// Includes from project
#include "objectsFactory.hpp"
#include "../src/physics/sphereCollider.hpp"
#include "../src/physics/meshCollider.hpp"

// Includes from STL
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;

/*
* Load a 3D object, by loading a .obj file
* 
* @param object3d Object3D to load
* @param pRenderingInstance Rendering instance of the object
* @param pGl3dWidget OpenGl3DWidget to add the object
* @param folderName Folder name of the object
* @param fileName File name of the object
* @param pos Position of the object
* @param scale Scale of the object
* @return void
*/
bool ObjectsFactory::load3dObject(
	Object3D& object3d,
	std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
	OpenGl3DWidget* pGl3dWidget,
	const std::string& folderName,
	const std::string& fileName,
	Vec3 pos,
	Vec3 scale
)
{
	const fs::path texturePath = g_pReourcesPath / fs::path(folderName) / fileName;

	// Load the object
	bool ret = object3d.loadFromObjFile(folderName, fileName);
	
	// Add the object to the rendering list
	if (ret)
	{
		pRenderingInstance = pGl3dWidget->addObject(object3d);
		pRenderingInstance->m_pPosRotScale->m_position = pos.toArray();
		pRenderingInstance->m_pPosRotScale->m_scale = scale.toArray();
	}

	return ret;
}


/*
* Create a 3D object, by loading a .obj file
* 
* @param object3d Object3D to create
* @param pRenderingInstance Rendering instance of the object
* @param pCollider Collider of the object
* @param pGl3dWidget OpenGl3DWidget to add the object
* @param folderName Folder name of the object
* @param fileName File name of the object
* @param pos Position of the object
* @param scale Scale of the object
* @return void
*/
void ObjectsFactory::create3dObject(
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
	bool ret = load3dObject(object3d, pRenderingInstance, pGl3dWidget, folderName, fileName, pos, scale);
	if (!ret)
	{
		return;
	}

	// Create the collider
	pCollider = std::make_shared<MeshCollider>(pos, object3d);
	if (!pCollider)
	{
		std::cerr << "Error: Failed to create the collider" << std::endl;
	}
}


/*
* Create a sphere, by loading a sphere.obj file
* 
* @param object3d Object3D to create
* @param pRenderingInstance Rendering instance of the object
* @param pCollider Collider of the object
* @param pGl3dWidget OpenGl3DWidget to add the object
* @param pos Position of the sphere
* @param radius Radius of the sphere
* @return void
*/
void ObjectsFactory::createSphere(
	Object3D& object3d,
	std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
	std::shared_ptr<Collider>& pCollider,
	OpenGl3DWidget* pGl3dWidget,
	Vec3 pos,
	double radius
)
{
	// Load the object
	object3d.loadFromObjFile("../models/sphere_highRes/", "sphere.obj");

	// Add the object to the rendering list
	pRenderingInstance = pGl3dWidget->addObject(object3d);
	pRenderingInstance->m_pPosRotScale->m_position = pos.toArray();
	pRenderingInstance->m_pPosRotScale->m_scale = {1.0, 1.0, 1.0};

	// Create the collider
	pCollider = std::make_shared<SphereCollider>(pos, radius);
}