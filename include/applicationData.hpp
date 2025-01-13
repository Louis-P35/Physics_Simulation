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
	Object3D m_cube3D;
	std::shared_ptr<ObjectHandle> m_pCube3DHandle;
	Object3D m_ground3D;
	std::shared_ptr<ObjectHandle> m_pGround3DHandle;
	Object3D m_debugSphere3D;
	std::shared_ptr<ObjectHandle> m_pDebugSphere3DHandle;

	//Particle m_testParticle;
	//Particle m_testParticle2;
	std::shared_ptr<Cloth> m_pCloth;

private:
	std::chrono::steady_clock::time_point m_lastUpdateTime;

public:
	ApplicationData();
	~ApplicationData() {};

	void initSimulation();
	bool simulationUpdate();
};