#pragma once

// Includes from project
#include "../src/math/vec3.hpp"
#include "../src/physics/aabb.hpp"

// Includes from STL
#include <vector>
#include <unordered_map>
#include <mutex>
#include <tuple>


class GridCell
{
public:
	// UID, index I, index J
	std::vector<std::tuple<std::string, int, int>> m_particlesId;
};


/*
* Class GridCollider
* 
* This class is used to represent a grid of cells
* Each cell contains a list of particles
* The grid is used to optimize collisions detections between particles
*/
class GridCollider
{
private:
	// Size of the cells
	double m_step;
	std::mutex m_mutex;
	std::unordered_map<size_t, GridCell> m_grid[2];

public:
	int m_readGrid = 0;
	int m_writeGrid = 1;

public:
	GridCollider(const double step) : m_step(step) {};
	~GridCollider() {};

	inline size_t hashKey(const int x, const int y, const int z) const;
	inline void getCellCoords(const Vec3& position, int& x, int& y, int& z) const;
	GridCell* getCell(const int x, const int y, const int z);
	void addParticleToCell(const Vec3& position, const std::tuple<std::string, int, int>& particleId);
	void swap();

private:
	void clearGrid(const int index);
};