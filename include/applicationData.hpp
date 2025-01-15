#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/physics/particle.hpp"
#include "../src/physics/cloth.hpp"

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
	std::shared_ptr<ObjectRenderingInstance> m_pCube3DRenderer;
	Object3D m_ground3D;
	std::shared_ptr<ObjectRenderingInstance> m_pGround3DRenderer;
	Object3D m_debugSphere3D;
	std::shared_ptr<ObjectRenderingInstance> m_pDebugSphere3DRenderer;

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