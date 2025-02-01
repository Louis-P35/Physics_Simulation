// Includes from project
#include "../src/physics/gridCollider.hpp"

// Includes from STL
#include <functional>

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
	x = static_cast<int>(round(position.x / m_step));
	y = static_cast<int>(round(position.y / m_step));
	z = static_cast<int>(round(position.z / m_step));
}


StaticGridCollider::StaticGridCollider(const double step, const size_t with, const size_t height, const Vec3& orig) : GridCollider(step),
m_gridWidth(with), m_gridHeight(height), m_gridWidthHeight(with* height), m_gridOrigine(orig)
{
	// Allocate the grid
	size_t size = m_gridWidth * m_gridWidth * m_gridHeight;
	m_gridWrite.resize(size);
	m_gridRead.resize(size);
}


/*
* Get the index of the cell at the specified coordinates
* Because the grid is a 3D array stored in a 1D vector, we need to compute the index
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return size_t Index of the cell in the grid
*/
inline size_t StaticGridCollider::getCellIndex(const int x, const int y, const int z) const
{
	return x + y * m_gridWidth + z * m_gridWidthHeight;
}

/*
* Check if the coordinates are valid
* 
* @param x X coordinate
* @param y Y coordinate
* @param z Z coordinate
* @return bool True if the coordinates are valid, false otherwise
*/
inline bool StaticGridCollider::isCoordValid(const int x, const int y, const int z) const
{
	return x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridWidth && z >= 0 && z < m_gridHeight;
}


/*
* Add a particle to the cell corresponding to the specified position
* 
* @param position Position of the particle
* @param particleId Particle Id to add (cloth uid + index I + index J)
* @return void
*/
void StaticGridCollider::addParticleToCell(const Vec3& position, const std::tuple<size_t, int, int>& particleId)
{
	int x;
	int y;
	int z;

	// Get the cell coordinates
	getCellCoords(position, x, y, z);

	// Check if the coordinates are not out of bounds
	if (isCoordValid(x, y, z))
	{
		// Get the index of the cell in the grid
		int index = getCellIndex(x, y, z);

		// Lock the mutex to prevent concurrent access when writing to the grid
		std::lock_guard<std::mutex> lock(m_mutex);

		// Insert it into the grid
		if (index < m_gridWrite.size())
		{
			m_gridWrite[index].m_particlesId.push_back(particleId);

			// If the cell is just created, store it's pointer to the list of non-empty cells of the write grid
			// Also if the cell is just created, store it's coordinates
			if (m_gridWrite[index].m_particlesId.size() == 1)
			{
				m_listOfPointerToNonEmptyCellsWrite.push_back(&m_gridWrite[index]);

				m_gridWrite[index].x = x;
				m_gridWrite[index].y = y;
				m_gridWrite[index].z = z;
			}
		}
	}
}


/*
* Get the cell at the specified coordinates
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return GridCell* Pointer to the cell
*/
GridCell* StaticGridCollider::getCell(const int x, const int y, const int z)
{
	// No mutex for read operations (because reading and writing are done in two differents grids)

	// Check if the coordinates are not out of bounds
	if (isCoordValid(x, y, z))
	{
		// Get the index of the cell in the grid
		int index = getCellIndex(x, y, z);

		// Return the cell
		if (index < m_gridRead.size())
		{
			return &m_gridRead[index];
		}
	}

	return nullptr;
}


/*
* Swap the read and write grids
* Very fast, just internal vectors' pointers swich
* Also clear the grid for the next iteration
* 
* @return void
*/
void StaticGridCollider::swap()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Clear the read grid
	clearGrid();

	// Swap the read & write grids
	m_gridWrite.swap(m_gridRead);

	// Swap the list of pointers to non-empty cells of the read grid
	m_listOfPointerToNonEmptyCellsWrite.swap(m_listOfPointerToNonEmptyCellsRead);

	// From that point, the write grid is empty and can be used to write new data,
	// while the read grid is full with the previous data and can be used to read data
}


/*
* Clear each cell of the read grid without deallocating the grid memory.
* This function is called in parallel to clear the grid faster.
*
* @return void
*/
void StaticGridCollider::clearGridParallelized(const size_t indexFrom, const size_t indexTo)
{
	// Clear only the non-empty cells of the read grid to avoid iterating over all the cells
	for (size_t i = indexFrom; i < indexTo; ++i)
	{
		m_listOfPointerToNonEmptyCellsRead[i]->m_particlesId.clear();
	}

	// m_listOfPointerToNonEmptyCellsRead need to be cleared after to clear the list of pointers to non-empty cells
}


/*
* Clear each cell of the read grid without deallocating the grid memory
*
* @return void
*/
void StaticGridCollider::clearGrid()
{
	// Only called from swap() that already lock the mutex

	// Clear each cell of the read grid
	// TODO: Optimize this by multithreading or using m_listOfPointerToNonEmptyCells
	/*for (auto& cell : m_gridRead)
	{
		cell.m_particlesId.clear();
	}*/

	// Clear only the non-empty cells of the read grid to avoid iterating over all the cells
	for (auto& cell : m_listOfPointerToNonEmptyCellsRead)
	{
		cell->m_particlesId.clear();
	}

	// Clear the list of pointers to non-empty cells of the read grid
	m_listOfPointerToNonEmptyCellsRead.clear();
}


/*
* Get the memory size of the grid collider
*
* @return size_t Memory size
*/
size_t StaticGridCollider::getMemorySize()
{
	size_t memorySize = 0;

	for (auto& cell : m_gridRead)
	{
		memorySize += sizeof(cell);
	}

	for (auto& cell : m_gridWrite)
	{
		memorySize += sizeof(cell);
	}

	for (auto& cell : m_listOfPointerToNonEmptyCellsRead)
	{
		memorySize += sizeof(cell);
	}

	for (auto& cell : m_listOfPointerToNonEmptyCellsWrite)
	{
		memorySize += sizeof(cell);
	}

	return memorySize;
}




/* 
* Helper function to compute a unique hash key from (x, y, z)
* 
* @param x X coordinate
* @param y Y coordinate
* @param z Z coordinate
* @return size_t Unique hash key
*/
inline size_t HashGridCollider::hashKey(const int x, const int y, const int z) const
{
    size_t h1 = std::hash<int>()(x);
    size_t h2 = std::hash<int>()(y);
    size_t h3 = std::hash<int>()(z);

	// Using the XOR bitwise operator to combine the hashes and minimize keys collisions
    return h1 ^ (h2 << 1) ^ (h3 << 2);
}


/*
* Swap the read and write grids
* Very fast, just internal vectors' pointers swich
* Also clear the grid for the next iteration
*
* @return void
*/
void HashGridCollider::swap()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	// Clear the read grid
	clearGrid();

	// Swap the read & write grids
	m_gridWrite.swap(m_gridRead);

	// From that point, the write grid is empty and can be used to write new data,
	// while the read grid is full with the previous data and can be used to read data
}


/* 
* Clear the read grid
* 
* @return void
*/
void HashGridCollider::clearGrid()
{
	// Only called from swap() that already lock the mutex

    m_gridRead.clear();
}

/* 
* Get the cell (from the read grid) at the specified coordinates
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return GridCell* Pointer to the cell
*/
GridCell* HashGridCollider::getCell(const int x, const int y, const int z)
{
	// No mutex for read operations (because reading and writing are done in two differents grids)
	
	// Get the hash corresponding to the cell coordinates
	size_t key = hashKey(x, y, z);

	// Find the cell in the grid
	auto it = m_gridRead.find(key);
	if (it != m_gridRead.end())
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
void HashGridCollider::addParticleToCell(const Vec3& position, const std::tuple<size_t, int, int>& particleId)
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
	m_gridWrite[key].m_particlesId.push_back(particleId);
	
	// If the cell is just created, store it's coordinates
	if (m_gridWrite[key].m_particlesId.size() == 1)
	{
		m_gridWrite[key].x = x;
		m_gridWrite[key].y = y;
		m_gridWrite[key].z = z;
	}
}


/*
* Get the memory size of the grid collider
* 
* @return size_t Memory size
*/
size_t HashGridCollider::getMemorySize()
{
	size_t memorySize = 0;

	for (auto& cell : m_gridRead)
	{
		memorySize += sizeof(cell);
	}

	for (auto& cell : m_gridWrite)
	{
		memorySize += sizeof(cell);
	}

	return memorySize;
}