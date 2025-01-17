// Include from project
#include "cloth.hpp"

// Includes from STL
#include <iostream>


Cloth::~Cloth()
{
	std::cout << "Cloth destroyed" << std::endl;
}


Cloth::Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position) :
	m_resX(resX), m_resY(resY), m_width(width), m_height(height), m_thickness(thickness), m_clothMass(clothMass), m_position(position)
{
	const int nbParticles = m_resX * m_resY * 2.0;
	const double particleMass = clothMass / static_cast<double>(nbParticles);

	// Create particles
	for (int i = 0; i < m_resX; ++i)
	{
		std::vector<Particle> rowBottom;
		std::vector<Particle> rowTop;
		for (int j = 0; j < m_resY; ++j)
		{
			Vec3 posBottom = Vec3(m_position.x + static_cast<double>(i) * m_width / m_resX, m_position.y, m_position.z + static_cast<double>(j) * m_height / m_resY);
			rowBottom.push_back(Particle(posBottom, particleMass));

			Vec3 posTop = Vec3(m_position.x + static_cast<double>(i) * m_width / m_resX, m_position.y + thickness, m_position.z + static_cast<double>(j) * m_height / m_resY);
			rowTop.push_back(Particle(posTop, particleMass));
		}

		m_particlesBottom.push_back(rowBottom);
		m_particlesTop.push_back(rowTop);
	}

	// Define the spring parameters
	const double springStrengh = 500.0;
	const double springDamping = 0.0;

	// Create the springs
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			const double distanceBetweenSame = (m_particlesTop[i][j].m_position - m_particlesBottom[i][j].m_position).norm();
			m_particlesBottom[i][j].m_springs.push_back(Spring(&m_particlesTop[i][j], distanceBetweenSame, springStrengh, springDamping));
			m_particlesTop[i][j].m_springs.push_back(Spring(&m_particlesBottom[i][j], distanceBetweenSame, springStrengh, springDamping));

			// Loop over the neighbors
			for (int ii = i - 1; ii <= i + 1; ++ii)
			{
				for (int jj = j - 1; jj <= j + 1; ++jj)
				{
					if ((ii == i && jj == j) || ii < 0 || ii >= m_resX || jj < 0 || jj >= m_resY)
					{
						continue;
					}
					// Compute the distance between the particles, it is the same for top and bottom
					const double distanceSameLayer = (m_particlesBottom[i][j].m_position - m_particlesBottom[ii][jj].m_position).norm();
					const double distanceBetweenLayer = (m_particlesTop[i][j].m_position - m_particlesBottom[ii][jj].m_position).norm();

					// Add the neighbor
					m_particlesBottom[i][j].m_springs.push_back(Spring(&m_particlesBottom[ii][jj], distanceSameLayer, springStrengh, springDamping));
					// Add the neighbor top particles
					m_particlesBottom[i][j].m_springs.push_back(Spring(&m_particlesTop[ii][jj], distanceBetweenLayer, springStrengh, springDamping));

					// Add the neighbor
					m_particlesTop[i][j].m_springs.push_back(Spring(&m_particlesTop[ii][jj], distanceSameLayer, springStrengh, springDamping));
					// Add the neighbor top particles
					m_particlesTop[i][j].m_springs.push_back(Spring(&m_particlesBottom[ii][jj], distanceBetweenLayer, springStrengh, springDamping));
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
* @return void
*/
void Cloth::updateSimulation(const std::vector<std::shared_ptr<Collider>>& colliders)
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
	updateParticles(elapsedTimeInSeconds, colliders);

	// Update the cloth's mesh
	updateMesh();
}


/*
* Update the cloth by updating all the particles
* 
* @param dt Time step
* @return void
*/
void Cloth::updateParticles(double dt, const std::vector<std::shared_ptr<Collider>>& colliders)
{
	// Clamp the time step to avoid huge time steps
	// This is a simple way to avoid instability in the simulation
	if (dt > 0.1)
	{
		dt = 0.1;
	}


	// Update the particles
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			m_particlesBottom[i][j].update(dt, colliders);
			m_particlesTop[i][j].update(dt, colliders);
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
			m_particlesTop[i][j].m_previousPosition = m_particlesTop[i][j].m_position;
			m_particlesTop[i][j].m_previousVelocity = m_particlesTop[i][j].m_velocity;

			// Update mesh vertices
			m_object3D.m_vertices[i * m_resY + j] = m_particlesBottom[i][j].m_position.toArray(); // Bottom side
			m_object3D.m_vertices[offsetTopBottom + i * m_resY + j] = m_particlesTop[i][j].m_position.toArray(); // Top side
		}
	}
}


/*
* Initialize the mesh of the cloth
* 
* @return void
*/
void Cloth::initMesh()
{
	// Create the vertices and normals for the bottom and top faces
	initMeshOneFace(0, m_particlesBottom);
	m_meshNbFacesOneSide = m_object3D.m_faces.size();
	initMeshOneFace(m_object3D.m_vertices.size(), m_particlesTop);
	initMeshSides();

	// Load textures, Compute the tangent and bitangent vectors
	m_object3D.postProcess("", false, false);
}


/*
* Initialize the mesh of the cloth with one face (the top one or the bottom one)
* 
* @param offset Offset to apply to the vertices
* @param topBottomFace List of particles for the top or bottom face
* @return void
*/
void Cloth::initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace)
{
	// Create the vertices
	for (int i = 0; i < m_resX; ++i)
	{
		for (int j = 0; j < m_resY; ++j)
		{
			m_object3D.m_vertices.push_back(topBottomFace[i][j].m_position.toArray());
			m_object3D.m_normals.push_back({ 0.0f, 1.0f, 0.0f }); // TODO
		}
	}

	// Create the faces
	for (int i = 0; i < m_resX - 1; ++i)
	{
		for (int j = 0; j < m_resY - 1; ++j)
		{
			const int vertexIndexCurrent = offset + i * m_resY + j;

			const int vertexIndexRight = offset + i * m_resY + j + 1;
			const int vertexIndexTop = offset + (i + 1) * m_resY + j;

			const int vertexIndexTopRight = offset + (i + 1) * m_resY + j + 1;

			// Bottom face
			if (vertexIndexRight >= 0 && vertexIndexRight < m_object3D.m_vertices.size() &&
				vertexIndexTop >= 0 && vertexIndexTop < m_object3D.m_vertices.size())
			{
				m_object3D.m_faces.push_back(
					{ vertexIndexCurrent, -1, vertexIndexCurrent,
					vertexIndexRight, -1, vertexIndexRight,
					vertexIndexTop, -1, vertexIndexTop }
				);
			}
			// Top face
			if (vertexIndexRight >= 0 && vertexIndexRight < m_object3D.m_vertices.size() &&
				vertexIndexTop >= 0 && vertexIndexTop < m_object3D.m_vertices.size() &&
				vertexIndexTopRight >= 0 && vertexIndexTopRight < m_object3D.m_vertices.size())
			{
				m_object3D.m_faces.push_back(
					{ vertexIndexRight, -1, vertexIndexRight,
					vertexIndexTopRight, -1, vertexIndexTopRight,
					vertexIndexTop, -1, vertexIndexTop }
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

			m_object3D.m_faces.push_back(
				{ vertexIndexCurrent, -1, vertexIndexCurrent,
				vertexIndexRight, -1, vertexIndexRight,
				vertexIndexTop, -1, vertexIndexTop }
			);

			const int vertexIndexTopRight = nbVerticesPerFace + i * m_resY + (j + 1);

			m_object3D.m_faces.push_back(
				{ vertexIndexTop, -1, vertexIndexTop,
				vertexIndexRight, -1, vertexIndexRight,
				vertexIndexTopRight, -1, vertexIndexTopRight }
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

			m_object3D.m_faces.push_back(
				{ vertexIndexCurrent, -1, vertexIndexCurrent,
				vertexIndexRight, -1, vertexIndexRight,
				vertexIndexTop, -1, vertexIndexTop }
			);

			const int vertexIndexTopRight = nbVerticesPerFace + (i + 1) * m_resY + j;

			m_object3D.m_faces.push_back(
				{ vertexIndexTop, -1, vertexIndexTop,
				vertexIndexRight, -1, vertexIndexRight,
				vertexIndexTopRight, -1, vertexIndexTopRight }
			);
		}
	};

	// Create the sides
	createSide1Lambda(0);
	createSide1Lambda(m_resX - 1);
	createSide2Lambda(0);
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

	// Recompute the normals
	for (int i = 0; i < m_object3D.m_faces.size(); ++i)
	{
		Vec3 p0 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][0]]);
		Vec3 p1 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][3]]);
		Vec3 p2 = Vec3(m_object3D.m_vertices[m_object3D.m_faces[i][6]]);

		Vec3 normal = (p1 - p0).cross(p2 - p0).getNormalized();
		// Invert the normal for the bottom face
		if (i < m_meshNbFacesOneSide)
		{
			normal *= -1.0;
		}

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
