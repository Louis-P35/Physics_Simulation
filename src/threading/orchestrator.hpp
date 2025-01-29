#pragma once

// Includes from project
#include "applicationData.hpp"

// Includes from STL
#include <thread>
#include <atomic>
#include <memory>



/*
* Class Orchestrator
* This class is a singleton that orchestrates the simulation
* It is responsible for starting and stopping the simulation
* and for managing the simulation's threads
*/
class Orchestrator
{
private:
	std::thread m_orchestratorThread;
	ApplicationData* m_pAppData;


public:
	Orchestrator();
	~Orchestrator() {};

	static Orchestrator& getInstance();

	void runOrchestrator();
	void start(ApplicationData& appData);
	void stop();
};