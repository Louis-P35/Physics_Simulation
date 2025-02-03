#pragma once

// Includes from project
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/view/OpenGl/objectRenderingInstance.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/math/vec3.hpp"
#include "../src/physics/sphereCollider.hpp"

// Includes from 3rd party

// Includes from STL
#include <memory>
#include <string>

/*
* ObjectsFactory class
* 
* This class is used to load 3D objects and to create 3d primitives
* Also compute the colliders
*/
class ObjectsFactory
{
public:
	constexpr static const char* g_pReourcesPath = "../models/";

public:
	ObjectsFactory() = delete;
	~ObjectsFactory() = delete;

	static void create3dObject(
		Object3D& object3d,
		std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
		std::shared_ptr<Collider>& pCollider,
		OpenGl3DWidget* pGl3dWidget,
		const std::string& folderName,
		const std::string& fileName,
		Vec3 pos,
		Vec3 scale
	);

	static void createSphere(
		Object3D& object3d,
		std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
		std::shared_ptr<Collider>& pCollider,
		OpenGl3DWidget* pGl3dWidget,
		Vec3 pos,
		double radius
	);

private:
	static bool load3dObject(
		Object3D& object3d,
		std::shared_ptr<ObjectRenderingInstance>& pRenderingInstance,
		OpenGl3DWidget* pGl3dWidget,
		const std::string& folderName,
		const std::string& fileName,
		Vec3 pos,
		Vec3 scale
	);
};