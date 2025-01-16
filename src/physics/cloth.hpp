#pragma once

// Includes from project
#include "particle.hpp"
#include "../src/math/vec3.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"

// Includes from STL
#include <vector>


/*
* Class Cloth
* The cloth is made of particles, each particle is connected to its neighbors by springs
* The cloth is one or two grid of particles (depending on its thickness)
* Cloth is derived from Object3D because it is a 3D object
*/
class Cloth : public Object3D
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

	std::shared_ptr<ObjectRenderingInstance> m_pRenderingInstance;

public:
	Cloth(int resX, int resY, double width, double height, double thickness, double clothMass, Vec3 position);
	virtual ~Cloth() {};

	void update(double dt);
	void updateMesh();

private:
	void initMesh();
	void initMeshOneFace(const int offset, const std::vector<std::vector<Particle>>& topBottomFace);
};