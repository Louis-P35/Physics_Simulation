#pragma once

// Includes from project
#include "applicationData.hpp"
#include "../src/threading/taskQueue.hpp"

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
	std::vector<std::thread> m_workerThreads;
	ApplicationData* m_pAppData;
	TaskQueue m_taskQueue;

public:
	Orchestrator(const size_t numberOfThreads);
	~Orchestrator() {};

	static Orchestrator& getInstance();

	void runOrchestrator();
	void start(ApplicationData& appData);
	void stop();
};