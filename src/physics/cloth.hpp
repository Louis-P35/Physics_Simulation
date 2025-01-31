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

	const std::string m_UID;

private:
	int m_meshFaceIndexTop = 0;

	// Cloth texture params
	std::string m_textureFolderName = "3";
	float m_uvScale = 1.0f;

public:
	Cloth(int resX, int resY, double width, double height, double colliderRadius, double thickness, double clothMass, Vec3 position, std::string uid);
	virtual ~Cloth();

	void updatePreviousPositionAndVelocity(const int resxFrom, const int resxTo);

	static bool areParticlesNeighbors(const std::string& uid1, const std::string& uid2, const int i1, const int j1, const int i2, const int j2);
	void updateMesh();

	void updateParticles(
		const double dt,
		const int resxFrom,
		const int resxTo,
		const std::vector<std::shared_ptr<Collider>>& colliders,
		std::shared_ptr<GridCollider> pGridCollider
	);

private:
	void initMesh();
	void initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace, const bool isTop);
	std::shared_ptr<OctreeNode> createCollisionTree(std::shared_ptr<OctreeNode> pRoot, const int iMin, const int iMax, const int jMin, const int jMax);
};



class ClothesList
{
public:
	std::map<std::string, std::shared_ptr<Cloth>> m_pClothsMap;
	std::mutex m_mutex;

public:
	ClothesList() {};
	~ClothesList() {};

	void addCloth(std::shared_ptr<Cloth> pCloth);
	std::shared_ptr<Cloth> getCloth(const std::string& uid);
	void clearClothes();
};
