// Includes from project
#include "../src/physics/gridCollider.hpp"

// Includes from STL
#include <functional>

/* 
* Helper function to compute a unique hash key from(x, y, z)
* 
* @param x X coordinate
* @param y Y coordinate
* @param z Z coordinate
* @return size_t Unique hash key
*/
inline size_t GridCollider::hashKey(const int x, const int y, const int z) const
{
    size_t h1 = std::hash<int>()(x);
    size_t h2 = std::hash<int>()(y);
    size_t h3 = std::hash<int>()(z);

	// Using the XOR bitwise operator to combine the hashes and minimize keys collisions
    return h1 ^ (h2 << 1) ^ (h3 << 2);
}


/* 
* Helper function to calculate grid cell coordinates
* 
* @param position Position of the particle
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return void
*/
inline void GridCollider::getCellCoords(const Vec3& position, int& x, int& y, int& z) const
{
    x = static_cast<int>(floor(position.x / m_step));
    y = static_cast<int>(floor(position.y / m_step));
    z = static_cast<int>(floor(position.z / m_step));
}


/* 
* Clear the grid
* 
* @return void
*/
void GridCollider::clearGrid()
{
    m_grid.clear();
}

/* 
* Get the cell at the specified coordinates
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return GridCell* Pointer to the cell
*/
GridCell* GridCollider::getCell(const int x, const int y, const int z)
{
	// Get the hash corresponding to the cell coordinates
	size_t key = hashKey(x, y, z);

	// Find the cell in the grid
	auto it = m_grid.find(key);
	if (it != m_grid.end())
	{
		return &it->second;
	}

	return nullptr;
}


/*
* Add a particle to the cell corresponding to the specified position
* 
* @param position Position of the particle
* @param particleCol ParticleCollider to add
* @return void
*/
void GridCollider::addParticleToCell(const Vec3& position, const ParticleCollider& particleCol)
{
	int x;
	int y;
	int z;

	// Get the cell coordinates
	getCellCoords(position, x, y, z);

	// Get the hash corresponding to the cell coordinates
	size_t key = hashKey(x, y, z);

	// Find the cell in the grid (or create it) and add the particle to it
	m_grid[key].m_particlesColliders.push_back(particleCol);
}