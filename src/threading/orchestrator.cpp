// Includes from project
#include "../src/threading/orchestrator.hpp"

// Includes from STL
#include <iostream>


Orchestrator::Orchestrator()
{

}


/*
* The orchestrator class is a singleton
* This method return it's only instance
* 
* @return Orchestrator& The orchestrator instance
*/
Orchestrator& Orchestrator::getInstance()
{
	static Orchestrator instance;
	return instance;
}


/*
* Start the orchestrator by launching the orchestrator thread (main simulation thread)
* only if it is not running yet. It can have only one instance running.
* 
* @return void
*/
void Orchestrator::start(ApplicationData& appData)
{
	// Launch the orchestrator thread if it is not running yet
	if (!m_orchestratorThread.joinable())
	{
		m_pAppData = &appData; // m_pAppData will be valid 
		m_orchestratorThread = std::thread(&Orchestrator::runOrchestrator, this);
	}
	else
	{
		std::cerr << "Error: Orchestrator is already running" << std::endl;
	}
}

void Orchestrator::stop()
{
	// Stop the orchestrator thread
	if (m_orchestratorThread.joinable())
	{
		m_orchestratorThread.join();
	}
}


void Orchestrator::runOrchestrator()
{
	while (true)
	{
		//std::cout << "Orchestrator running: " << m_pAppData->m_colliders.size() << std::endl;
		m_pAppData->updateSimulation();
	}
}