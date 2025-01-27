#pragma once

// Includes from project
#include "../src/physics/cloth.hpp"
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/physics/collider.hpp"
#include "../src/physics/gridCollider.hpp"
#include "../src/dynamicBarrier.hpp"

// Includes from 3rd party

// Includes from STL
#include <memory>
#include <vector>
#include <map>


/*
* Class ClothFactory
* 
* This class is used to create a cloth,
* add the mesh of the cloth to the rendering widget,
* and start the simulation in a separate thread
*/
class ClothFactory
{
public:
	static DynamicBarrier s_barrier;

public:
	ClothFactory() = delete;
	~ClothFactory() = delete;

	static std::shared_ptr<Cloth> createCloth(
		int resX,
		int resY,
		double width,
		double height,
		double thickness,
		double clothMass,
		Vec3 position,
		OpenGl3DWidget* pOpenGl3DWidget,
		std::vector<std::shared_ptr<Collider>>& colliders,
		std::shared_ptr<GridCollider> pGridCollider,
		ClothesList& pCloths
	);

private:
	static std::string generateUID();
};