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
protected:
	// Size of the cells
	double m_step;
	std::mutex m_mutex;

public:
	GridCollider(const double step) : m_step(step) {};
	~GridCollider() {};

	inline void getCellCoords(const Vec3& position, int& x, int& y, int& z) const;
	virtual void clearGrid() = 0;
	virtual GridCell* getCell(const int x, const int y, const int z) = 0;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<std::string, int, int>& particleId) = 0;
};


class StaticGridCollider : public GridCollider
{
private:
	Vec3 m_min;
	Vec3 m_max;
	std::vector<std::vector<std::vector<GridCell>>> m_grid;

public:
	StaticGridCollider(const double step) : GridCollider(step) {};
	~StaticGridCollider() {};

	virtual GridCell* getCell(const int x, const int y, const int z) override;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<std::string, int, int>& particleId) override;
	virtual void clearGrid() override;
};


class HashGridCollider : public GridCollider
{
private:
	std::unordered_map<size_t, GridCell> m_grid;

public:
	HashGridCollider(const double step) : GridCollider(step) {};
	~HashGridCollider() {};

	inline size_t hashKey(const int x, const int y, const int z) const;
	virtual GridCell* getCell(const int x, const int y, const int z) override;
	virtual void addParticleToCell(const Vec3& position, const std::tuple<std::string, int, int>& particleId) override;
	virtual void clearGrid() override;
};