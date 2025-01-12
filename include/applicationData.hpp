#pragma once

// Includes from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/OpenGl/object3D.hpp"
#include "particle.hpp"

// Includes from STL
#include <memory>
#include <vector>
#include <chrono>


class ApplicationData
{
public:
	std::vector<std::unique_ptr<Object3D>> m_objects3D;
	Object3D m_ground3D;
	Object3D m_debugSphere3D;

	Particle m_testParticle;

private:
	std::chrono::steady_clock::time_point m_lastUpdateTime;

public:
	ApplicationData();
	~ApplicationData() {};

	bool simulationUpdate();
};