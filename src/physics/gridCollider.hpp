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
	std::vector<std::tuple<size_t, int, int>> m_particlesId;
	int x;
	int y;
	int z;
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
protected:
	// Size of the cells
	double m_step;
	std::mutex m_mutex;

public:
	GridCollider(const double step) : m_step(step) {};
	~GridCollider() {};

	inline void getCellCoords(const Vec3& position, int& x, int& y, int& z) const;
	virtual void clearGrid() = 0;
	virtual void clearGridParallelized(const size_t indexFrom, const size_t indexTo) = 0;
	virtual GridCell* getCell(const int x, const int y, const int z) = 0;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<size_t, int, int>& particleId) = 0;
	virtual void swap() = 0;
	virtual size_t getMemorySize() = 0;
};


class StaticGridCollider : public GridCollider
{
private:
	Vec3 m_gridOrigine;
	size_t m_gridWidth; // Left & right
	size_t m_gridHeight; // Up
	size_t m_gridWidthHeight; // optimization: width * height

public:
	std::vector<GridCell> m_gridWrite;
	std::vector<GridCell> m_gridRead;
	std::vector<GridCell*> m_listOfPointerToNonEmptyCellsWrite;
	std::vector<GridCell*> m_listOfPointerToNonEmptyCellsRead;

public:
	StaticGridCollider(const double step, const size_t with, const size_t height, const Vec3& orig);
	~StaticGridCollider() {};

	inline bool isCoordValid(const int x, const int y, const int z) const;
	inline size_t getCellIndex(const int x, const int y, const int z) const;
	virtual GridCell* getCell(const int x, const int y, const int z) override;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<size_t, int, int>& particleId) override;
	virtual void swap() override;
	virtual size_t getMemorySize() override;
	virtual void clearGridParallelized(const size_t indexFrom, const size_t indexTo) override;

private:
	virtual void clearGrid() override;
};


class HashGridCollider : public GridCollider
{
public:
	std::unordered_map<size_t, GridCell> m_gridWrite;
	std::unordered_map<size_t, GridCell> m_gridRead;

public:
	HashGridCollider(const double step) : GridCollider(step) {};
	~HashGridCollider() {};

	inline size_t hashKey(const int x, const int y, const int z) const;
	virtual GridCell* getCell(const int x, const int y, const int z) override;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<size_t, int, int>& particleId) override;
	virtual void swap() override;
	virtual size_t getMemorySize() override;
	virtual void clearGridParallelized(const size_t indexFrom, const size_t indexTo) override;

private:
	virtual void clearGrid() override;
};