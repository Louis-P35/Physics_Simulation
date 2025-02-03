#pragma once

// Includes from project
#include "particle.hpp"
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/physics/collider.hpp"
#include "../src/physics/octree.hpp"
#include "../src/physics/gridCollider.hpp"

// Includes from STL
#include <vector>
#include <mutex>
#include <shared_mutex>


class ClothesList;


/*
* Class Cloth
* The cloth is made of particles, each particle is connected to its neighbors by springs
* The cloth is one or two grid of particles (depending on its thickness)
*/
class Cloth //: public PhysicsWorker
{
public:
	int m_resX;
	int m_resY;
	double m_width;
	double m_height;
	double m_thickness;
	double m_clothMass;

	mutable std::mutex m_mutex;
	std::vector<std::vector<Particle>> m_particles;

	Object3D m_object3D;
	std::shared_ptr<ObjectRenderingInstance> m_pRenderingInstance;

	std::shared_ptr<OctreeNode> m_pCollisionTree;

	size_t m_uidIndex = -1;

private:
	int m_meshFaceIndexTop = 0;

	// Cloth texture params
	std::string m_textureFolderName = "3";
	float m_uvScale = 1.0f;

public:
	Cloth(
		int resX, int resY, 
		double width, double height, 
		double colliderRadius, 
		double thickness, 
		double clothMass, 
		Vec3 position
	);
	virtual ~Cloth();

	void updatePreviousPositionAndVelocity(const int resxFrom, const int resxTo);

	static bool areParticlesNeighbors(
		const size_t uidIndex1, 
		const size_t uidIndex2,
		const int i1, const int j1, 
		const int i2, const int j2
	);
	void updateMesh();

	void updateParticles(
		const double dt,
		const int resxFrom,
		const int resxTo,
		const std::vector<std::shared_ptr<Collider>>& colliders,
		std::shared_ptr<GridCollider> pGridCollider
	);

	void updateGridCollider(std::shared_ptr<GridCollider> pGridCollider, const int indexFrom, const int indexTo);

private:
	void initMesh();
	void initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace, const bool isTop);
};



class ClothesList
{
public:
	std::vector<std::shared_ptr<Cloth>> m_pCloths;
	mutable std::shared_mutex m_addClothMutex;

public:
	ClothesList() {};
	~ClothesList() {};

	void addCloth(std::shared_ptr<Cloth> pCloth);
	std::shared_ptr<Cloth> getCloth(const size_t uidIndex);
	void clearClothes();
};
