#pragma once

// Includes from project
#include "cloth.hpp"
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/physics/collider.hpp"

// Includes from 3rd party

// Includes from STL
#include <memory>
#include <vector>


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
	ClothFactory() {};
	~ClothFactory() {};

	static std::shared_ptr<Cloth> createCloth(
		int resX, 
		int resY, 
		double width, 
		double height, 
		double thickness, 
		double clothMass, 
		Vec3 position,
		OpenGl3DWidget* pOpenGl3DWidget,
		std::vector<std::shared_ptr<Collider>>& colliders
	);
};