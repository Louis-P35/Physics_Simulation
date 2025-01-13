#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "particle.hpp"
#include "cloth.hpp"

// Includes from STL
#include <memory>
#include <vector>
#include <chrono>

/*
* ApplicationData class
* 
* This class is used to store the data of the application (model)
*/
class ApplicationData
{
public:
	std::vector<std::unique_ptr<Object3D>> m_objects3D;
	Object3D m_ground3D;
	Object3D m_debugSphere3D;

	//Particle m_testParticle;
	//Particle m_testParticle2;
	std::shared_ptr<Cloth> m_pCloth;

private:
	std::chrono::steady_clock::time_point m_lastUpdateTime;

public:
	ApplicationData();
	~ApplicationData() {};

	bool simulationUpdate();
};