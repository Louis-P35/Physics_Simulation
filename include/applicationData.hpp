#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/physics/particle.hpp"
#include "../src/physics/cloth.hpp"
#include "physicsWorker.hpp"

// Includes from 3rd party
#include <QObject>

// Includes from STL
#include <memory>
#include <vector>
#include <chrono>

/*
* ApplicationData class
* 
* This class is used to store the data of the application (model)
*/
class ApplicationData : public QObject
{
	Q_OBJECT

public:
	OpenGl3DWidget* m_pOpenGl3DWidget;

	Object3D m_cube3D;
	std::shared_ptr<ObjectRenderingInstance> m_pCube3DRenderer;
	Object3D m_ground3D;
	std::shared_ptr<ObjectRenderingInstance> m_pGround3DRenderer;
	Object3D m_debugSphere3D;
	std::shared_ptr<ObjectRenderingInstance> m_pDebugSphere3DRenderer;

	//std::shared_ptr<Cloth> m_pCloth;
	std::vector<std::shared_ptr<Cloth>> m_pCloths;

private:
	std::chrono::steady_clock::time_point m_lastUpdateTime;

public:
	ApplicationData();
	~ApplicationData() {};

	bool initSimulation();
	bool simulationUpdate();
	bool resetSimulation();
};