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
		Vec3(3.0, 1.0, 3.0),
		1.0
	);
	m_colliders.push_back(pCollider);

	/*Object3D sphere3D;
	std::shared_ptr<ObjectRenderingInstance> pSphere3DRenderer;
	std::shared_ptr<Collider> pCollider2 = nullptr;
	ObjectsFactory::createSphere(
		sphere3D,
		pSphere3DRenderer,
		pCollider2,
		pGl3dWidget,
		Vec3(-1.0, 3.0, -2.5),
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
		Vec3(-2.20, 1.0, -2.5),
		1.0
	);
	m_colliders.push_back(pCollider3);*/
	

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
	size_t gridWith = static_cast<size_t>(6.0 / cellSize);
	size_t gridHeight = static_cast<size_t>(5.0 / cellSize);
	//m_pGridCollider = std::make_shared<HashGridCollider>(cellSize, gridWith, gridHeight, Vec3(0.0, 0.0, 0.0));
	m_pGridCollider = std::make_shared<HashGridCollider>(cellSize);
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
	Vec3 position = Vec3(2.75, 3.0, 2.75);
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

	Vec3 position2 = Vec3(3.0, 3.6, 3.0);
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

	/*Vec3 position3 = Vec3(-1.0, 3.1, -1.0);
	std::shared_ptr<Cloth> pCloth3 = ClothFactory::createCloth(
		res, res, 
		sideSize, sideSize,
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
	}*/


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
	// TODO

	// No need of mutex from here because threads are stopped
	
	// Remove all the cloths from the rendering widget
	for (auto& [uid, pCloth] : m_pCloths.m_pClothsMap)
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


void ApplicationData::updateSimulation()
{
	// First, update all the cloths' particles and the collisions with static colliders
	// Add the particles to the hash grid collider
	for (auto& [uid, pCloth] : m_pCloths.m_pClothsMap)
	{
		if (pCloth)
		{
			pCloth->updateSimulation(m_colliders, m_pGridCollider);
		}
	}

	// From here, all particles' position and previousPosition are the same
	// So we can resolve the collisions between the particles using the particles' previousPosition

	auto t1 = std::chrono::steady_clock::now();

	// Then, resolve the collisions between the cloths
	if (m_pGridCollider)
	{
		// Loop over all the non-empty (existing) grid cells
		for (auto& cell : m_pGridCollider->m_gridRead)
		{
			std::vector<std::tuple<Particle*, Particle*>> pairs;

			// Loop over all the pair of particles in that cell
			for (int i = 0; i < cell.second.m_particlesId.size(); ++i)
			{
				// Get the particles Id
				auto& [clothUID1, partI1, partJ1] = cell.second.m_particlesId[i];
				// Get the cloths
				auto pCloth1 = m_pCloths.getCloth(clothUID1);

				for (int j = i + 1; j < cell.second.m_particlesId.size(); ++j)
				{
					// Get the particles Id
					auto& [clothUID2, partI2, partJ2] = cell.second.m_particlesId[j];

					// Skip the current particle and the neightbors if we collide to ourself
					if (Cloth::areParticlesNeighbors(clothUID1, clothUID2, partI1, partJ1, partI2, partJ2))
					{
						continue;
					}

					// Get the cloths
					auto pCloth2 = m_pCloths.getCloth(clothUID2);

					// Add the pair of particles to the list
					if (pCloth1 && pCloth2)
					{
						pairs.push_back(std::make_tuple(
							&pCloth1->m_particles[partI1][partJ1],
							&pCloth2->m_particles[partI2][partJ2]
						));
					}
				}

				// Loop over the adjacent cells
				// But not all, to avoid checking twice two particles that are in two differents cells
				// Like so:
				//  Top     Middle   Bottom   : Z axis
				// 0 0 0    0 0 0    1 1 1
				// 0 0 0    0 0 1    1 1 1
				// 0 0 0    1 1 1    1 1 1
				for (int xx = cell.second.x - 1; xx <= cell.second.x + 1; ++xx)
				{
					for (int yy = cell.second.y - 1; yy <= cell.second.y + 1; ++yy)
					{
						for (int zz = cell.second.z - 1; zz <= cell.second.z; ++zz)
						{
							// Skip the current cell
							if (xx == cell.second.x && yy == cell.second.y && zz == cell.second.z)
							{
								continue;
							}

							// Skip the 'x' cell:
							// Y
							// 0 0 0
							// x 0 1
							// 1 1 1  X
							if (zz == cell.second.z && xx == cell.second.x - 1 && yy == cell.second.y)
							{
								continue;
							}

							// Skip the 'x' cell:
							// Y
							// x x x
							// 0 0 1
							// 1 1 1  X
							if (zz == cell.second.z && yy == cell.second.y + 1)
							{
								continue;
							}

							// Get the adjacent cell
							GridCell* pAdjCell = m_pGridCollider->getCell(xx, yy, zz);
							if (pAdjCell) // If the adjacent cell exist (has particles)
							{
								// Loop over the particles Id in that cell
								for (auto& [clothUID2, partI2, partJ2] : pAdjCell->m_particlesId)
								{
									// Skip the current particle and the neightbors if we collide to ourself
									if (Cloth::areParticlesNeighbors(clothUID1, clothUID2, partI1, partJ1, partI2, partJ2))
									{
										continue;
									}

									// Get the cloths
									auto pCloth2 = m_pCloths.getCloth(clothUID2);

									// Add the pair of particles to the list
									if (pCloth1 && pCloth2)
									{
										pairs.push_back(std::make_tuple(
											&pCloth1->m_particles[partI1][partJ1],
											&pCloth2->m_particles[partI2][partJ2]
										));
									}
								}
							}
						}
					}
				}
			}

			// Loop over the pairs of particles and resolve the collisions
			for (auto& [pPart1, pPart2] : pairs)
			{
				if (pPart1 && pPart2)
				{
					Particle::detectCollision(*pPart1, *pPart2);
				}
			}
		}

		static bool start = false;
		auto t2 = std::chrono::steady_clock::now();
		std::chrono::duration<float> deltaTime = t2 - t1;
		float elapsedTimeInSeconds = deltaTime.count();
		static std::vector<float> times;
		if (start)
		{
			times.push_back(elapsedTimeInSeconds);
		}
		float average = 0.0;
		for (const auto& t : times)
		{
			average += t;
		}
		average /= static_cast<float>(times.size());

		// Update previosPositions
		for (auto& [uid, pCloth] : m_pCloths.m_pClothsMap)
		{
			if (pCloth)
			{
				// Loop through all the particles
				for (int i = 0; i < pCloth->m_resX; ++i)
				{
					for (int j = 0; j < pCloth->m_resY; ++j)
					{
						pCloth->m_particles[i][j].m_previousPosition = pCloth->m_particles[i][j].m_position;
					}
				}
			}
		}

		// Swap the read and write grids
		m_pGridCollider->swap();
	}
}