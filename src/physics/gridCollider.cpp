// Includes from project
#include "../src/physics/gridCollider.hpp"

// Includes from STL
#include <functional>

/* 
* Helper function to compute a unique hash key from (x, y, z)
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
void GridCollider::clearGrid(const int index)
{
    m_grid[index].clear();
}

/* 
* Get the cell (from the read grid) at the specified coordinates
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return GridCell* Pointer to the cell
*/
GridCell* GridCollider::getCell(const int x, const int y, const int z)
{
	// No mutex for read operations (because reading and writing are done in two differents grids)
	
	// Get the hash corresponding to the cell coordinates
	size_t key = hashKey(x, y, z);

	// Find the cell in the grid
	auto it = m_grid[m_readGrid].find(key);
	if (it != m_grid[m_readGrid].end())
	{
		return &it->second;
	}

	return nullptr;
}


/*
* Add a particle to the cell corresponding to the specified position (on the write grid)
* 
* @param position Position of the particle
* @param particleId Particle Id to add (cloth uid + index I + index J)
* @return void
*/
void GridCollider::addParticleToCell(const Vec3& position, const std::tuple<std::string, int, int>& particleId)
{
	int x;
	int y;
	int z;

	// Get the cell coordinates
	getCellCoords(position, x, y, z);

	// Get the hash corresponding to the cell coordinates
	size_t key = hashKey(x, y, z);

	// Lock the mutex to prevent concurrent access to the grid
	std::lock_guard<std::mutex> lock(m_mutex);

	// Find the cell in the grid (or create it) and add the particle to it
	m_grid[m_writeGrid][key].m_particlesId.push_back(particleId);
}


/*
* Swap the reading and writing grid
* Clear the write grid
* 
* @return void
*/
void GridCollider::swap()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Swap the read and write grid
	m_readGrid = m_writeGrid;
	m_writeGrid = 1 - m_readGrid;

	// Clear the new write grid for the next iteration
	clearGrid(m_writeGrid);
}