// Include from project
#include "cloth.hpp"

// Includes from STL
#include <iostream>
#include <filesystem>
#include <tuple>


namespace fs = std::filesystem;


Cloth::~Cloth()
{
	std::cout << "Cloth destroyed" << std::endl;
}


Cloth::Cloth(int resX, int resY, double width, double height, double colliderRadius, double thickness, double clothMass, Vec3 position, std::string uid) :
	m_resX(resX), m_resY(resY), m_width(width), m_height(height), m_thickness(thickness), m_clothMass(clothMass), m_UID(uid)
{
	const int nbParticles = m_resX * m_resY;
	const double particleMass = clothMass / static_cast<double>(nbParticles);
	const double distBetweenParticlesX = m_width / static_cast<double>(m_resX - 1);
	const double distBetweenParticlesY = m_height / static_cast<double>(m_resY - 1);

	Vec3 pos = position - Vec3(m_width / 2.0, 0.0, m_height / 2.0);

	// Create particles
	for (int i = 0; i < m_resX; ++i)
	{
		std::vector<Particle> rowBottom;
		std::vector<Particle> rowTop;

		for (int j = 0; j < m_resY; ++j)
		{
			// Create the particles
			Vec3 posBottom = Vec3(pos.x + static_cast<double>(i) * distBetweenParticlesX, pos.y, pos.z + static_cast<double>(j) * distBetweenParticlesY);
			Particle particleBottom = Particle(posBottom, particleMass);
			particleBottom.m_indexI = i;
			particleBottom.m_indexJ = j;
			
			Vec3 posTop = Vec3(pos.x + static_cast<double>(i) * distBetweenParticlesX, pos.y + thickness, pos.z + static_cast<double>(j) * distBetweenParticlesY);
			Particle particleTop = Particle(posTop, particleMass);
			
			// Create the AABB for the particle (only for the bottom side)
			particleBottom.m_pAabb = std::make_shared<AABB>(colliderRadius);
			particleBottom.m_pAabb->constructCubicAABB(posBottom);

			// Add the particles to the lists
			rowBottom.push_back(particleBottom);
			rowTop.push_back(particleTop);
		}

		m_particlesBottom.push_back(rowBottom);
		m_particlesTop.push_back(rowTop); // Still needed for creating the mesh
	}

	// Define the spring parameters
	const double springStrengh = 1000.0;
	const double springDamping = 0.0;

	// Create the springs
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			const int distEffect = 2;
			// Loop over the neighbors
			for (int ii = i - distEffect; ii <= i + distEffect; ++ii)
			{
				for (int jj = j - distEffect; jj <= j + distEffect; ++jj)
				{
					// Skip the current particle (and the indexes outside the grid)
					if ((ii == i && jj == j) || ii < 0 || ii >= m_resX || jj < 0 || jj >= m_resY)
					{
						continue;
					}

					const int dist = std::max(std::abs(ii - i), std::abs(jj - j)); // dist can not be 0
					const double strenghDecreaseFactor = 2.0;
					const double springStrenghLocal = springStrengh / (static_cast<double>(dist) * strenghDecreaseFactor);

					// Compute the distance between the particles, it is the same for top and bottom
					const double distanceSameLayer = (m_particlesBottom[i][j].m_position - m_particlesBottom[ii][jj].m_position).norm();

					// Add the neighbor
					m_particlesBottom[i][j].m_springs.push_back(Spring(&m_particlesBottom[ii][jj], distanceSameLayer, springStrenghLocal, springDamping));
				}
			}

			// Debug
			if (j == (m_resY - 1) || j == 0 || i == 0 || i == (m_resX - 1))
			{
				//m_particlesBottom[i][j].setFixed(true);
				//m_particlesTop[i][j].setFixed(true);
			}
		}
	}

	// Initialize the mesh
	initMesh();

	// Initialize the last update time
	m_lastUpdateTime = std::chrono::steady_clock::now();
}


/*
* Update the cloth's physics (all particles) and mesh
* 
* @param colliders The list of colliders in the scene
* @param pGridCollider The hash grid collider instance
* @param pCloths The map of cloth in the scene
* @return void
*/
void Cloth::updateSimulation(
	const std::vector<std::shared_ptr<Collider>>& colliders, 
	std::shared_ptr<GridCollider> pGridCollider//,
	//ClothesList& pCloths
)
{
	static bool firstUpdate = true;

	// Calculate the time elapsed since the last update
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> deltaTime = currentTime - m_lastUpdateTime;
	m_lastUpdateTime = currentTime;

	// Skip the first update to avoid a huge time step
	if (firstUpdate)
	{
		firstUpdate = false;
		return;
	}

	// Convert deltaTime to seconds
	float elapsedTimeInSeconds = deltaTime.count();

	// Update the simulation
	updateParticles(elapsedTimeInSeconds, colliders, pGridCollider/*, pCloths */);

	// Update the cloth's mesh
	updateMesh();
}


/*
* Update the cloth by updating all the particles
*
* @param colliders The list of colliders in the scene
* @param pGridCollider The hash grid collider instance
* @param pCloths The map of cloth in the scene
* @param dt Time step
* @return void
*/
void Cloth::updateParticles(
	double dt, 
	const std::vector<std::shared_ptr<Collider>>& colliders, 
	std::shared_ptr<GridCollider> pGridCollider
)
{
	// Clamp the time step to avoid huge time steps
	// This is a simple way to avoid instability in the simulation
	if (dt > 0.005)
	{
		dt = 0.005;
	}

	auto t1 = std::chrono::steady_clock::now();

	// Update the particles
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			// Update the particles
			m_particlesBottom[i][j].update(dt, colliders);

			// Handle collision with the ground
			if (m_particlesBottom[i][j].m_position.y < m_particlesBottom[i][j].m_pAabb->m_halfSize)
			{
				m_particlesBottom[i][j].m_position.y = m_particlesBottom[i][j].m_pAabb->m_halfSize;

				m_particlesBottom[i][j].bounceOnCollision(Vec3(0.0, 1.0, 0.0), 1.0 / m_particlesBottom[i][j].m_groundFriction);
			}

			// Handle collision with the colliders
			for (const auto& pCollider : colliders)
			{
				if (!pCollider)
				{
					continue;
				}

				Vec3 collPosition;
				Vec3 collNormal;
				Vec3 bounceVect;
				if (pCollider->hasCollided(collPosition, collNormal, bounceVect, m_particlesBottom[i][j].m_previousPosition, m_particlesBottom[i][j].m_position, m_particlesBottom[i][j].m_pAabb->m_halfSize))
				{
					// Compute the new position
					m_particlesBottom[i][j].m_position = collPosition + collNormal * m_particlesBottom[i][j].m_pAabb->m_halfSize;
					// Compute the new velocity
					m_particlesBottom[i][j].m_velocity = bounceVect * m_particlesBottom[i][j].m_velocity.norm();
					if (m_particlesBottom[i][j].m_objectFriction > 0.0)
					{
						m_particlesBottom[i][j].m_velocity *= 1.0 / m_particlesBottom[i][j].m_objectFriction;
					}
				}
			}

			// Update the AABB
			m_particlesBottom[i][j].m_pAabb->constructCubicAABB(m_particlesBottom[i][j].m_position);

			// Add the particle's new position to the grid collider
			if (pGridCollider)
			{
				pGridCollider->addParticleToCell(
					m_particlesBottom[i][j].m_position, 
					std::make_tuple(m_UID, i, j)
				);
			}
		}
	}

	// Update the previous position and velocity
	const int offsetTopBottom = m_resX * m_resY;
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			m_particlesBottom[i][j].m_previousPosition = m_particlesBottom[i][j].m_position;
			m_particlesBottom[i][j].m_previousVelocity = m_particlesBottom[i][j].m_velocity;
		}
	}
}



/*
* Check if two particles are neighbors
* 
* @param uid1 The first particle's cloth UID
* @param uid2 The second particle's cloth UID
* @param i1 The first particle's i index
* @param j1 The first particle's j index
* @param i2 The second particle's i index
* @param j2 The second particle's j index
* 
* @return bool True if the particles are neighbors, false otherwise
*/
bool Cloth::areParticlesNeighbors(const std::string& uid1, const std::string& uid2, const int i1, const int j1, const int i2, const int j2)
{
	if (uid1 == uid2)
	{
		int distNoEffect = 2;

		bool isJneighbor = (j1 == j2);
		bool isIneighbor = (i1 == i2);

		for (int k = 1; k < (distNoEffect + 1); ++k)
		{
			isJneighbor |= (j2 == (j1 - k) || j2 == (j1 + k));
			isIneighbor |= (i2 == (i1 - k) || i2 == (i1 + k));
		}

		if (isIneighbor && isJneighbor)
		{
			return true;
		}
	}

	return false;
}


/*
* Create the collision tree for the cloth
* Recursively climb down the tree to create the children
* When it reach a leaf, create it's bounding box
* Then climb back up to set the AABB of the currents nodes (fuse the AABB of the children)
* 
* @param pRoot The current node of the tree
* @param iMin The minimum index in the X direction
* @param iMax The maximum index in the X direction
* @param jMin The minimum index in the Y direction
* @param jMax The maximum index in the Y direction
* @return std::shared_ptr<OctreeNode> The newly created node
*/
std::shared_ptr<OctreeNode> Cloth::createCollisionTree(
	std::shared_ptr<OctreeNode> pRoot, 
	const int iMin, 
	const int iMax, 
	const int jMin, 
	const int jMax
)
{
	if (!pRoot)
	{
		return nullptr;
	}

	// At this point, pRoot is not yet addes to the tree

	// If is leaf
	if (iMin == iMax && jMin == jMax)
	{
		// Set the AABB of the leaf to be the AABB of the particle
		pRoot->setAabb(*m_particlesBottom[iMin][jMin].m_pAabb);

		// Store the indexes of the particle to avoid collision with itself or its neighbors
		pRoot->m_indexI = iMin;
		pRoot->m_indexJ = jMin;

		// Return the leaf
		return pRoot;
	}


	// Compute the middle indexes
	int iMiddle = (iMin + iMax) / 2;
	int jMiddle = (jMin + jMax) / 2;

	// Going down the tree, we create the children of the current node
	// At this point pRoot can not be a leaf
	std::shared_ptr<OctreeNode> pChild1 = std::make_shared<OctreeNode>(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
	pRoot->addChildren(createCollisionTree(pChild1, iMin, iMiddle, jMin, jMiddle));
	if (jMin < jMax)
	{
		std::shared_ptr<OctreeNode> pChild2 = std::make_shared<OctreeNode>(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
		pRoot->addChildren(createCollisionTree(pChild2, iMin, iMiddle, jMiddle + 1, jMax));
		
	}
	if (iMin < iMax)
	{
		std::shared_ptr<OctreeNode> pChild3 = std::make_shared<OctreeNode>(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
		pRoot->addChildren(createCollisionTree(pChild3, iMiddle + 1, iMax, jMin, jMiddle));
		
	}
	if (jMin < jMax && iMin < iMax)
	{
		std::shared_ptr<OctreeNode> pChild4 = std::make_shared<OctreeNode>(Vec3(0.0, 0.0, 0.0), Vec3(0.0, 0.0, 0.0));
		pRoot->addChildren(createCollisionTree(pChild4, iMiddle + 1, iMax, jMiddle + 1, jMax));
	}

	// Climbing back up the tree, we set the AABB of the current node
	// Fuse the AABB of the children
	std::vector<std::shared_ptr<AABB>> aabbs;
	for (const auto& pChild : pRoot->m_pChildren)
	{
		aabbs.push_back(std::dynamic_pointer_cast<AABB>(pChild));
	}
	pRoot->setAabb(aabbs);

	return pRoot;
}


/*
* Initialize the mesh of the cloth
* 
* @return void
*/
void Cloth::initMesh()
{
	// Create the vertices and normals for the bottom and top faces
	initMeshOneFace(0, m_particlesBottom, false);
	m_meshFaceIndexTop = m_object3D.m_faces.size();
	initMeshOneFace(m_object3D.m_vertices.size(), /*m_particlesTop*/m_particlesBottom, true);
	//initMeshSides();

	// Load textures, Compute the tangent and bitangent vectors
	const fs::path texturePath = fs::path("../models/fabrics_textures/") / m_textureFolderName;
	m_object3D.postProcess(texturePath.string(), true, true);
}


/*
* Initialize the mesh of the cloth with one face (the top one or the bottom one)
* 
* @param offset Offset to apply to the vertices
* @param topBottomFace List of particles for the top or bottom face
* @return void
*/
void Cloth::initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace, const bool isTop)
{
	// Create the vertices, normals, UVs
	for (int i = 0; i < m_resX; ++i)
	{
		const float u = (static_cast<float>(i) / static_cast<float>(m_resX - 1)) * m_uvScale;

		for (int j = 0; j < m_resY; ++j)
		{
			// vertices
			m_object3D.m_vertices.push_back(topBottomFace[i][j].m_position.toArray());
			// Normals
			m_object3D.m_normals.push_back({ 0.0f, 1.0f, 0.0f });
			// UVs
			const float v = (static_cast<float>(j) / static_cast<float>(m_resY - 1)) * m_uvScale;
			m_object3D.m_uvs.push_back({ u, v });
		}
	}

	// Create the faces
	for (int i = 0; i < m_resX - 1; ++i)
	{
		for (int j = 0; j < m_resY - 1; ++j)
		{
			int vertexIndexCurrent = offset + i * m_resY + j;
			int vertexIndexRight = offset + i * m_resY + j + 1;
			int vertexIndexTop = offset + (i + 1) * m_resY + j;
			int vertexIndexTopRight = offset + (i + 1) * m_resY + j + 1;

			if (isTop && i == 0)
			{
				vertexIndexCurrent = i * m_resY + j;
				vertexIndexRight = i * m_resY + j + 1;
			}
			if (isTop && i == (m_resX - 2))
			{
				vertexIndexTop = (i + 1) * m_resY + j;
				vertexIndexTopRight = (i + 1) * m_resY + j + 1;
			}
			if (isTop && j == 0)
			{
				vertexIndexCurrent = i * m_resY + j;
				vertexIndexTop = (i + 1) * m_resY + j;
			}
			if (isTop && j == (m_resY - 2))
			{
				vertexIndexRight = i * m_resY + j + 1;
				vertexIndexTopRight = (i + 1) * m_resY + j + 1;
			}

			// Bottom face
			if (vertexIndexRight >= 0 && vertexIndexRight < m_object3D.m_vertices.size() &&
				vertexIndexTop >= 0 && vertexIndexTop < m_object3D.m_vertices.size())
			{
				m_object3D.m_faces.push_back(
					{ vertexIndexCurrent, vertexIndexCurrent, vertexIndexCurrent,
					vertexIndexRight, vertexIndexRight, vertexIndexRight,
					vertexIndexTop, vertexIndexTop, vertexIndexTop }
				);
			}
			// Top face
			if (vertexIndexRight >= 0 && vertexIndexRight < m_object3D.m_vertices.size() &&
				vertexIndexTop >= 0 && vertexIndexTop < m_object3D.m_vertices.size() &&
				vertexIndexTopRight >= 0 && vertexIndexTopRight < m_object3D.m_vertices.size())
			{
				m_object3D.m_faces.push_back(
					{ vertexIndexRight, vertexIndexRight, vertexIndexRight,
					vertexIndexTopRight, vertexIndexTopRight, vertexIndexTopRight,
					vertexIndexTop, vertexIndexTop, vertexIndexTop }
				);
			}
		}
	}
}


/*
* Initialize the 4 sides of the mesh of the cloth
* 
* @return void
*/
void Cloth::initMeshSides()
{
	// Lambda to create 2 sides
	auto createSide1Lambda = [this](const int i) {
		const int nbVerticesPerFace = m_resX * m_resY;

		for (int j = 0; j < m_resY - 1; ++j)
		{
			const int vertexIndexCurrent = i * m_resY + j;
			const int vertexIndexRight = i * m_resY + (j + 1);
			const int vertexIndexTop = nbVerticesPerFace + i * m_resY + j;

			// XXX: UVs are not correct ?
			m_object3D.m_faces.push_back(
				{ vertexIndexCurrent, vertexIndexCurrent, vertexIndexCurrent,
				vertexIndexRight, vertexIndexRight, vertexIndexRight,
				vertexIndexTop, vertexIndexTop, vertexIndexTop }
			);

			const int vertexIndexTopRight = nbVerticesPerFace + i * m_resY + (j + 1);

			// XXX: UVs are not correct ?
			m_object3D.m_faces.push_back(
				{ vertexIndexTop, vertexIndexTop, vertexIndexTop,
				vertexIndexRight, vertexIndexRight, vertexIndexRight,
				vertexIndexTopRight, vertexIndexTopRight, vertexIndexTopRight }
			);
		}
	};

	// Lambda to create the 2 others sides on the other axis
	auto createSide2Lambda = [this](const int j) {
		const int nbVerticesPerFace = m_resX * m_resY;

		for (int i = 0; i < m_resX - 1; ++i)
		{
			const int vertexIndexCurrent = i * m_resY + j;
			const int vertexIndexRight = (i + 1) * m_resY + j;
			const int vertexIndexTop = nbVerticesPerFace + i * m_resY + j;

			// XXX: UVs are not correct ?
			m_object3D.m_faces.push_back(
				{ vertexIndexCurrent, vertexIndexCurrent, vertexIndexCurrent,
				vertexIndexRight, vertexIndexRight, vertexIndexRight,
				vertexIndexTop, vertexIndexTop, vertexIndexTop }
			);

			const int vertexIndexTopRight = nbVerticesPerFace + (i + 1) * m_resY + j;

			// XXX: UVs are not correct ?
			m_object3D.m_faces.push_back(
				{ vertexIndexTop, vertexIndexTop, vertexIndexTop,
				vertexIndexRight, vertexIndexRight, vertexIndexRight,
				vertexIndexTopRight, vertexIndexTopRight, vertexIndexTopRight }
			);
		}
	};

	// Create the sides
	m_meshFaceIndexSide1 = m_object3D.m_faces.size();
	createSide1Lambda(0);
	m_meshFaceIndexSide2 = m_object3D.m_faces.size();
	createSide1Lambda(m_resX - 1);
	m_meshFaceIndexSide3 = m_object3D.m_faces.size();
	createSide2Lambda(0);
	m_meshFaceIndexSide4 = m_object3D.m_faces.size();
	createSide2Lambda(m_resY - 1);
}


/*
* Update the mesh vertices
* Need to be called after the update of the particles and before the rendering
* 
* @return void
*/
void Cloth::updateMesh()
{
	if (!m_pRenderingInstance)
	{
		return;
	}

	// Update mesh vertices
	const int offsetTopBottom = m_resX * m_resY;
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			Vec3 p0 = m_particlesBottom[i][j].m_position;
			int nextI = i + 1;
			int nextJ = j + 1;
			bool isInverted = false;
			if (nextI >= m_resX)
			{
				nextI = i - 1;
				isInverted = true;
			}
			if (nextJ >= m_resY)
			{
				nextJ = j - 1;
				isInverted = !isInverted; // XOR
			}
			Vec3 p1 = m_particlesBottom[nextI][j].m_position;
			Vec3 p2 = m_particlesBottom[i][nextJ].m_position;
			Vec3 normal = (p1 - p0).cross(p2 - p0).getNormalized();
			if (!isInverted)
			{
				normal = normal * -1.0;
			}

			//const bool isSide = (i == 0 || i == (m_resX - 1) || j == 0 || j == (m_resY - 1));
			const double halfThickness = m_thickness / 2.0;

			// Top side is updated on top of the particlesBottom's positions
			Vec3 posTop = m_particlesBottom[i][j].m_position + normal * halfThickness;
			/*if (isSide)
			{
				posTop = m_particlesBottom[i][j].m_position;
			}*/
			m_object3D.m_vertices[offsetTopBottom + i * m_resY + j] = posTop.toArray(); // Top side

			// Bottom side is updated below the particlesBottom's positions
			Vec3 posBottom = m_particlesBottom[i][j].m_position - normal * halfThickness;
			/*if (isSide)
			{
				posBottom = m_particlesBottom[i][j].m_position;
			}*/
			m_object3D.m_vertices[i * m_resY + j] = posBottom.toArray();
		}
	}

	// Recompute the normals
	for (int i = 0; i < m_object3D.m_faces.size(); ++i)
	{
		Vec3 p0 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][0]]);
		Vec3 p1 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][3]]);
		Vec3 p2 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][6]]);

		Vec3 normal = (p1 - p0).cross(p2 - p0).getNormalized();
		// Invert the normal for the bottom face
		if (i < m_meshFaceIndexTop)
		{
			normal *= -1.0;
		}
		
		/*if (i >= m_meshFaceIndexSide1 && i < m_meshFaceIndexSide2)
		{
			
		}

		if (i >= m_meshFaceIndexSide2 && i < m_meshFaceIndexSide3)
		{
			normal *= -1.0;
		}

		if (i >= m_meshFaceIndexSide3 && i < m_meshFaceIndexSide4)
		{
			normal *= -1.0;
		}

		if (i >= m_meshFaceIndexSide4)
		{
			normal *= -1.0;
		}*/

		m_object3D.m_normals[m_object3D.m_faces[i][2]] = normal.toArray();
		m_object3D.m_normals[m_object3D.m_faces[i][5]] = normal.toArray();
		m_object3D.m_normals[m_object3D.m_faces[i][8]] = normal.toArray();
	}

	// Lock the mutex to prevent the rendering thread to access the vertices data
	std::lock_guard<std::mutex> lock(m_pRenderingInstance->m_mutex);

	// TODO: Optize that! Do not realloc
	// Update the VBO vertices data
	m_pRenderingInstance->m_verticesData.clear();
	m_pRenderingInstance->m_verticesData = m_object3D.computeVBOVerticesData();
}



/*
* Add a cloth to the map
* 
* @param pCloth The cloth to add
* @return void
*/
void ClothesList::addCloth(std::shared_ptr<Cloth> pCloth)
{
	if (!pCloth)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);

	m_pClothsMap[pCloth->m_UID] = pCloth;
}


/*
* Get a cloth by its UID
* 
* @param uid The UID of the cloth
* @return std::shared_ptr<Cloth> The cloth
*/
std::shared_ptr<Cloth> ClothesList::getCloth(const std::string& uid)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto it = m_pClothsMap.find(uid);
	if (it != m_pClothsMap.end() && it->second)
	{
		return it->second;
	}

	return nullptr;
}


/*
* Clear all the clothes
* 
* @return void
*/
void ClothesList::clearClothes()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_pClothsMap.clear();
}
