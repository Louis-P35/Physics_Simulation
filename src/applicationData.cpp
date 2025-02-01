// Includes from project
#include "applicationData.hpp"
#include "../src/physics/sphereCollider.hpp"
#include "clothFactory.hpp"
#include "objectsFactory.hpp"
#include "../src/threading/orchestrator.hpp"

// Includes from STL
#include <iostream>
#include <vector>
#include <tuple>
#include <unordered_set>



ApplicationData::ApplicationData() : m_pOpenGl3DWidget(nullptr)
{
	
}


ApplicationData::~ApplicationData()
{
	// Stop the physics simulation for all the cloths
	// TODO
}


bool ApplicationData::initAfterOpenGl(OpenGl3DWidget* pGl3dWidget)
{
	if (!pGl3dWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return false;
	}

	//Object3D test;
	//m_monkey3D.loadFromObjFile("../models/test2/", "testCube.obj");

	//m_monkey3D.loadFromObjFile("../models/Susanne/", "suzanne.obj");
	//m_monkey3D.loadFromObjFile("../models/sphere/", "sphere.obj");

	//std::shared_ptr<Object3D> pCube = std::make_shared<Object3D>();
	//m_cube3D.loadFromObjFile("../models/cube/", "cube.obj");
	//m_pCube3DRenderer = pGl3dWidget->addObject(m_cube3D);
	//m_pCube3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//m_pCube3DRenderer->m_pPosRotScale->m_scale = { 0.3f, 0.3f, 0.3f };

	std::shared_ptr<Collider> pCollider = nullptr;
	ObjectsFactory::createSphere(
		m_cube3D,
		m_pCube3DRenderer,
		pCollider,
		pGl3dWidget,
		Vec3(5.0, 1.0, 5.0),
		1.0
	);
	m_colliders.push_back(pCollider);

	Object3D sphere3D;
	std::shared_ptr<ObjectRenderingInstance> pSphere3DRenderer;
	std::shared_ptr<Collider> pCollider2 = nullptr;
	ObjectsFactory::createSphere(
		sphere3D,
		pSphere3DRenderer,
		pCollider2,
		pGl3dWidget,
		Vec3(3.5, 3.0, 5.0),
		1.0
	);
	m_colliders.push_back(pCollider2);

	Object3D sphere3D2;
	std::shared_ptr<ObjectRenderingInstance> pSphere3DRenderer2;
	std::shared_ptr<Collider> pCollider3 = nullptr;
	ObjectsFactory::createSphere(
		sphere3D2,
		pSphere3DRenderer2,
		pCollider3,
		pGl3dWidget,
		Vec3(6.5, 3.0, 5.0),
		1.0
	);
	m_colliders.push_back(pCollider3);
	

	// Ground
	m_ground3D.loadFromObjFile("../models/ground_2/", "ground.obj");
	m_pGround3DRenderer = pGl3dWidget->addObject(m_ground3D);
	m_pGround3DRenderer->m_pPosRotScale->m_position = { 0.0f, 0.0f, 0.0f };
	m_pGround3DRenderer->m_pPosRotScale->m_scale = { 1.0f, 1.0f, 1.0f };

	// Debug sphere
	m_debugSphere3D.loadFromObjFile("../models/sphere_highRes/", "sphere.obj");
	m_debugSphere3D2.loadFromObjFile("../models/sphere_highRes2/", "sphere.obj");
	m_debugSphere3D3.loadFromObjFile("../models/sphere_highRes3/", "sphere.obj");
	//m_pDebugSphere3DRenderer = pGl3dWidget->addObject(m_debugSphere3D);
	//m_pDebugSphere3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//m_pDebugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.1f, 0.1f, 0.1f };

	return initSimulation();
}



/*
* Initialize the simulation
* 
* @return bool True if the simulation was initialized successfully, false otherwise
*/
bool ApplicationData::initSimulation()
{
	if (!m_pOpenGl3DWidget)
	{
		std::cerr << "Error: OpenGl3DWidget is not initialized" << std::endl;
		return false;
	}

	//return true;

	const double particleRadius = 0.035;
	const int res = 30;

	const double particleColliderRadius = particleRadius * 2.0;
	double cellSize = particleColliderRadius * 2.0; // Must be at least equal to the particle collider diameter
	const double sideSize = particleRadius * 2.0 * static_cast<double>(res - 1);

	float scale = static_cast<float>(particleColliderRadius); // Sphere obj is 2.0 in diameter

	// Create the grid collider (10x10x10)
	size_t gridWith = static_cast<size_t>(10.0 / cellSize);
	size_t gridHeight = static_cast<size_t>(10.0 / cellSize);
	m_pGridCollider = std::make_shared<StaticGridCollider>(cellSize, gridWith, gridHeight, Vec3(0.0, 0.0, 0.0));
	//m_pGridCollider = std::make_shared<HashGridCollider>(cellSize);


	/*std::vector<Vec3> debugSpheres;
	Vec3 pos1 = Vec3(0.0, 0.0, 0.0);
	debugSpheres.push_back(pos1);
	debugSpheres.push_back(pos1 + Vec3(static_cast<double>(gridWith) * cellSize, 0.0, 0.0));
	debugSpheres.push_back(pos1 + Vec3(0.0, 0.0, static_cast<double>(gridWith) * cellSize));
	debugSpheres.push_back(pos1 + Vec3(static_cast<double>(gridWith) * cellSize, 0.0, static_cast<double>(gridWith) * cellSize));
	debugSpheres.push_back(pos1 + Vec3(0.0, static_cast<double>(gridHeight) * cellSize, 0.0));
	debugSpheres.push_back(pos1 + Vec3(static_cast<double>(gridWith) * cellSize, static_cast<double>(gridHeight) * cellSize, 0.0));
	debugSpheres.push_back(pos1 + Vec3(0.0, static_cast<double>(gridHeight) * cellSize, static_cast<double>(gridWith) * cellSize));
	debugSpheres.push_back(pos1 + Vec3(static_cast<double>(gridWith) * cellSize, static_cast<double>(gridHeight) * cellSize, static_cast<double>(gridWith) * cellSize));
	for (const auto& pos : debugSpheres)
	{
		std::shared_ptr<Collider> pCollider = nullptr;
		ObjectsFactory::createSphere(
			m_debugSphere3D,
			m_pDebugSphere3DRenderer,
			pCollider,
			m_pOpenGl3DWidget,
			pos,
			0.1
		);
		m_colliders.push_back(pCollider);
	}*/

	
	// Create a cloth
	Vec3 position = Vec3(5.75, 4.0, 4.75);
	std::shared_ptr<Cloth> pCloth = ClothFactory::createCloth(
		res, res, 
		sideSize, sideSize,
		particleColliderRadius,
		0.025, 300.0, 
		position, 
		m_pOpenGl3DWidget, 
		m_colliders,
		m_pGridCollider,
		m_pCloths
	);
	if (pCloth)
	{
		m_pCloths.addCloth(pCloth);
	}

	// Debug view of the particles
	/*for (int i = 0; i < pCloth->m_resX; ++i)
	{
		for (int j = 0; j < pCloth->m_resY; ++j)
		{
			pCloth->m_particles[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D3);
			pCloth->m_particles[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth->m_particles[i][j].m_position.toArray();
			pCloth->m_particles[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { scale, scale, scale };
		}
	}*/

	Vec3 position2 = Vec3(5.0, 4.6, 5.0);
	std::shared_ptr<Cloth> pCloth2 = ClothFactory::createCloth(
		res, res, 
		sideSize, sideSize,
		particleColliderRadius,
		0.025, 300.0, 
		position2, 
		m_pOpenGl3DWidget, 
		m_colliders, 
		m_pGridCollider, 
		m_pCloths
	);
	if (pCloth2)
	{
		m_pCloths.addCloth(pCloth2);
	}

	// Debug view of the particles
	/*for (int i = 0; i < pCloth2->m_resX; ++i)
	{
		for (int j = 0; j < pCloth2->m_resY; ++j)
		{
			pCloth2->m_particles[i][j].m_debugSphere3DRenderer = m_pOpenGl3DWidget->addObject(m_debugSphere3D2);
			pCloth2->m_particles[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_position = pCloth2->m_particles[i][j].m_position.toArray();
			pCloth2->m_particles[i][j].m_debugSphere3DRenderer->m_pPosRotScale->m_scale = { scale, scale, scale };
		}
	}*/

	Vec3 position3 = Vec3(6.25, 5.0, 5.0);
	std::shared_ptr<Cloth> pCloth3 = ClothFactory::createCloth(
		res, res,
		sideSize, sideSize,
		particleColliderRadius,
		0.025, 300.0,
		position3,
		m_pOpenGl3DWidget,
		m_colliders,
		m_pGridCollider,
		m_pCloths
	);
	if (pCloth3)
	{
		m_pCloths.addCloth(pCloth3);
	}


	Vec3 position4 = Vec3(4.0, 5.3, 5.0);
	std::shared_ptr<Cloth> pCloth4 = ClothFactory::createCloth(
		res, res,
		sideSize, sideSize,
		particleColliderRadius,
		0.025, 300.0,
		position4,
		m_pOpenGl3DWidget,
		m_colliders,
		m_pGridCollider,
		m_pCloths
	);
	if (pCloth4)
	{
		m_pCloths.addCloth(pCloth4);
	}


	Vec3 position5 = Vec3(4.5, 5.6, 5.0);
	std::shared_ptr<Cloth> pCloth5 = ClothFactory::createCloth(
		res*2, res*2,
		sideSize*2.0, sideSize*2.0,
		particleColliderRadius,
		0.025, 300.0,
		position5,
		m_pOpenGl3DWidget,
		m_colliders,
		m_pGridCollider,
		m_pCloths
	);
	if (pCloth5)
	{
		m_pCloths.addCloth(pCloth5);
	}


	// Start the physics simulation by starting the orchestrator (main simulation thread)
	Orchestrator::getInstance().start(*this);
}


/*
* Reset the simulation
* This function is called by a slot in the MainWindow class
* 
* @return void
*/
bool ApplicationData::resetSimulation()
{
	// Stop the physics simulation for all the cloths
	Orchestrator::getInstance().stop();

	// No need of mutex from here because threads are stopped
	
	// Remove all the cloths from the rendering widget
	for (auto& pCloth : m_pCloths.m_pCloths)
	{
		if (pCloth)
		{
			m_pOpenGl3DWidget->removeObject(pCloth->m_pRenderingInstance);
		}
	}

	// Clear the list of cloths
	m_pCloths.clearClothes();

	// Dirty, TODO fix this: Wait for the threads to finish
	_sleep(100);

	// Ensure the barrier threshold for threads synchronization is zero
	//ClothFactory::s_barrier.setThreshold(0);

	// Reset the and restart the simulation
	int res = initSimulation();

	return res;
}


/* 
* Hash function for unique particle pairs
* Cantor’s Pairing Function, uniquely encodes two natural numbers into a single integer
* 
* @param id1 The first particle Id
* @param id2 The second particle Id
* @return size_t The unique hash
*/
size_t getUniquePairHash(size_t id1, size_t id2)
{
	size_t a = std::min(id1, id2);
	size_t b = std::max(id1, id2);

	return ((a + b) * (a + b + 1)) / 2 + b;
}


/*
* Update the collisions between the particles
* This function detect between all pair of particles within the current cell and some of the adjacent cells
* Then resolve the collisions for each pair of particles
* 
* @param CellsFromReadGrid The list of non-empty grid cells (can be the entire list or just a batch for parallelism)
* @return void
*/
void ApplicationData::updateCollisions(const std::vector<std::shared_ptr<GridCell>>& CellsFromReadGrid)
{
	if (!m_pGridCollider) // Should not happend, but anyway...
	{
		return;
	}

	// Loop over all the non-empty (existing) grid cells.
	// Actually not all but the one in the list CellsFromReadGrid, to allow parallelism.
	for (auto& pCell : CellsFromReadGrid)
	{
		if (!pCell) // Should not happend, but anyway...
		{
			continue;
		}

		// Loop over all the pair of particles in that cell
		for (int i = 0; i < pCell->m_particlesId.size(); ++i)
		{
			// Get the particles Id
			auto& [clothUidIndex1, partI1, partJ1] = pCell->m_particlesId[i];
			// Get the cloths
			auto pCloth1 = m_pCloths.getCloth(clothUidIndex1);

			for (int j = i + 1; j < pCell->m_particlesId.size(); ++j)
			{
				// Get the particles Id
				auto& [clothUidIndex2, partI2, partJ2] = pCell->m_particlesId[j];

				// Skip the current particle and the neightbors if we collide to ourself
				if (Cloth::areParticlesNeighbors(clothUidIndex1, clothUidIndex2, partI1, partJ1, partI2, partJ2))
				{
					continue;
				}

				// Get the cloths
				auto pCloth2 = m_pCloths.getCloth(clothUidIndex2);

				// Resolve the collisions
				if (pCloth1 && pCloth2)
				{
					Particle::detectCollision(pCloth1->m_particles[partI1][partJ1], pCloth2->m_particles[partI2][partJ2]);
				}
			}

			// Loop over the adjacent cells
			// But not all, to avoid checking twice two particles that are in two differents cells
			// Like so:
			//  Top     Middle   Bottom   : Z axis
			// 0 0 0    0 0 0    1 1 1
			// 0 0 0    0 0 1    1 1 1
			// 0 0 0    1 1 1    1 1 1
			for (int xx = pCell->x - 1; xx <= pCell->x + 1; ++xx)
			{
				for (int yy = pCell->y - 1; yy <= pCell->y + 1; ++yy)
				{
					for (int zz = pCell->z - 1; zz <= pCell->z; ++zz)
					{
						// Skip the current cell
						if (xx == pCell->x && yy == pCell->y && zz == pCell->z)
						{
							continue;
						}

						// Skip the 'x' cell:
						// Y
						// 0 0 0
						// x 0 1
						// 1 1 1  X
						if (zz == pCell->z && xx == pCell->x - 1 && yy == pCell->y)
						{
							continue;
						}

						// Skip the 'x' cell:
						// Y
						// x x x
						// 0 0 1
						// 1 1 1  X
						if (zz == pCell->z && yy == pCell->y + 1)
						{
							continue;
						}

						// Get the adjacent cell
						std::shared_ptr<GridCell> pAdjCell = m_pGridCollider->getCell(xx, yy, zz);
						if (pAdjCell) // If the adjacent cell exist (has particles)
						{
							// Loop over the particles Id in that cell
							for (auto& [clothUID2, partI2, partJ2] : pAdjCell->m_particlesId)
							{
								// Skip the current particle and the neightbors if we collide to ourself
								if (Cloth::areParticlesNeighbors(clothUidIndex1, clothUID2, partI1, partJ1, partI2, partJ2))
								{
									continue;
								}

								// Get the cloths
								auto pCloth2 = m_pCloths.getCloth(clothUID2);

								// Resolve the collisions
								if (pCloth1 && pCloth2)
								{
									Particle::detectCollision(pCloth1->m_particles[partI1][partJ1], pCloth2->m_particles[partI2][partJ2]);
								}
							}
						}
					}
				}
			}
		}
	}
}


/*
* Stop the simulation and all the threads for a clean exit
* 
* @return void
*/
void ApplicationData::onApplicationExit()
{
	// Stop the physics simulation for all the cloths
	Orchestrator::getInstance().stop();
}