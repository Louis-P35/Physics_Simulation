#pragma once

// Includes from project
#include "particle.hpp"
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/physics/collider.hpp"
#include "../src/physics/octree.hpp"
#include "physicsWorker.hpp"
#include "../src/physics/gridCollider.hpp"

// Includes from STL
#include <vector>
#include <chrono>


/*
* Class Cloth
* The cloth is made of particles, each particle is connected to its neighbors by springs
* The cloth is one or two grid of particles (depending on its thickness)
*/
class Cloth : public PhysicsWorker
{
public:
	int m_resX;
	int m_resY;
	double m_width;
	double m_height;
	double m_thickness;
	double m_clothMass;

	Vec3 m_position;

	std::vector<std::vector<Particle>> m_particlesBottom;
	std::vector<std::vector<Particle>> m_particlesTop;

	Object3D m_object3D;
	std::shared_ptr<ObjectRenderingInstance> m_pRenderingInstance;

	std::shared_ptr<OctreeNode> m_pCollisionTree;

private:
	int m_meshFaceIndexTop = 0;
	int m_meshFaceIndexSide1 = 0;
	int m_meshFaceIndexSide2 = 0;
	int m_meshFaceIndexSide3 = 0;
	int m_meshFaceIndexSide4 = 0;
	std::chrono::steady_clock::time_point m_lastUpdateTime;

	// Cloth texture params
	std::string m_textureFolderName = "3";
	float m_uvScale = 1.0f;

public:
	Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position);
	virtual ~Cloth();

	void updateSimulation(const std::vector<std::shared_ptr<Collider>>& colliders, std::shared_ptr<GridCollider> pGridCollider);
	

private:
	void initMesh();
	void initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace);
	void initMeshSides();
	void handleCollisionWithItself(const int currentI, const int currentJ, std::shared_ptr<GridCollider> pGridCollider);
	void updateMesh();
	void updateParticles(double dt, const std::vector<std::shared_ptr<Collider>>& colliders, std::shared_ptr<GridCollider> pGridCollider);
	std::shared_ptr<OctreeNode> createCollisionTree(std::shared_ptr<OctreeNode> pRoot, const int iMin, const int iMax, const int jMin, const int jMax);
};