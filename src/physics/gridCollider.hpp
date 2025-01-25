#pragma once

// Includes from project
#include "../src/math/vec3.hpp"
#include "../src/physics/aabb.hpp"

// Includes from STL
#include <vector>
#include <unordered_map>

class ParticleCollider
{
public:
	Vec3 m_position;
	double m_radius;
	//AABB m_aabb; // TODO later

	ParticleCollider(const Vec3& position, const double radius) : m_position(position), m_radius(radius) {};
	~ParticleCollider() {};
};

class GridCell
{
public:
	std::vector<ParticleCollider> m_particlesColliders;
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

	std::unordered_map<size_t, GridCell> m_grid;

public:
	GridCollider(const double step) : m_step(step) {};
	~GridCollider() {};

	inline size_t hashKey(const int x, const int y, const int z) const;
	inline void getCellCoords(const Vec3& position, int& x, int& y, int& z) const;
	void clearGrid();
	GridCell* getCell(const int x, const int y, const int z);
	void addParticleToCell(const Vec3& position, const ParticleCollider& particleCol);
};