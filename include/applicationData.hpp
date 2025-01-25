#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/physics/particle.hpp"
#include "../src/physics/cloth.hpp"
#include "../src/physics/sphereCollider.hpp"
#include "physicsWorker.hpp"
#include "../src/physics/gridCollider.hpp"

// Includes from 3rd party
#include <QObject>

// Includes from STL
#include <memory>
#include <vector>

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

	std::vector<std::shared_ptr<Cloth>> m_pCloths;
	std::vector<std::shared_ptr<Collider>> m_colliders;
	std::shared_ptr<GridCollider> m_pGridCollider;

public:
	ApplicationData();
	~ApplicationData();

	bool initSimulation();
	bool resetSimulation();
	bool initAfterOpenGl(OpenGl3DWidget* pGl3dWidget);
};