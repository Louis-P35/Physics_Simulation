// Includes from project
#include "objectsFactory.hpp"
#include "../src/physics/sphereCollider.hpp"

// Includes from 3rd party

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
void ObjectsFactory::load3dObject(
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
	object3d.loadFromObjFile("../models/sphere_highRes/", "untitled.obj");
	
	// Add the object to the rendering list
	pRenderingInstance = pGl3dWidget->addObject(object3d);
	pRenderingInstance->m_pPosRotScale->m_position = pos.toArray();
	pRenderingInstance->m_pPosRotScale->m_scale = scale.toArray();
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
	load3dObject(object3d, pRenderingInstance, pGl3dWidget, folderName, fileName, pos, scale);

	// Create the collider (Sphere TODO change that)
	pCollider = std::make_shared<SphereCollider>(pos, 1.0);
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