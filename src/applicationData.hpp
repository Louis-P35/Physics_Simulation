#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/physics/particle.hpp"
#include "../src/physics/cloth.hpp"
#include "../src/physics/sphereCollider.hpp"
#include "../src/physics/gridCollider.hpp"

// Includes from 3rd party
#include <QObject>

// Includes from STL
#include <memory>
#include <vector>
#include <map>

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

	Object3D m_ground3D;
	std::vector<Object3D> m_3dObjects;

	// Map of cloths, the key is the cloth's UID
	ClothesList m_pCloths;

	// List of colliders in the scene
	std::vector<std::shared_ptr<Collider>> m_colliders;

	// The hash grid collision optimization system
	std::shared_ptr<StaticGridCollider> m_pGridCollider;

public:
	ApplicationData();
	~ApplicationData();

	bool initSimulation();
	bool resetSimulation();
	bool initAfterOpenGl(OpenGl3DWidget* pGl3dWidget);

	void onApplicationExit();

	// Simulation functions
	void updateCollisions(const std::vector<std::shared_ptr<GridCell>>& CellsFromReadGrid);
};