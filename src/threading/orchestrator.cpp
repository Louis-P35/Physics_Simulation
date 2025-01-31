// Includes from project
#include "../src/threading/orchestrator.hpp"

// Includes from STL
#include <iostream>


Orchestrator::Orchestrator(const size_t  numberOfThreads)
{
	// Worker thread lambda function
	// This just endlessley loops to gets and execute tasks from the task queue
	auto workerThreadLambda = [this]()
		{
			while (true)
			{
				std::function<void()> task;
				m_taskQueue.getTask(task);
				if (task)
				{
					task();
					m_taskQueue.markTaskAsDone();
				}
			}
		};

	// Create the worker threads
	for (size_t  i = 0; i < numberOfThreads; ++i)
	{
		m_workerThreads.push_back(std::thread(workerThreadLambda));
	}
}


/*
* The orchestrator class is a singleton
* This method return it's only instance
* 
* @return Orchestrator& The orchestrator instance
*/
Orchestrator& Orchestrator::getInstance()
{
	static Orchestrator instance(8);
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
	std::cout << "Orchestrator running" << std::endl;

	// Add tasks to the task queue
	m_taskQueue.addTask([this]() { std::cout << "Executing task 1" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 2" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 3" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 4" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 5" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 6" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 7" << std::endl; });
	m_taskQueue.addTask([this]() { std::cout << "Executing task 8" << std::endl; });

	std::cout << "Orchestrator waiting for tasks to be completed..." << std::endl;
	m_taskQueue.waitUntilEmpty();
	std::cout << "All tasks done! Orchestrator finised waiting" << std::endl;


	size_t i = 0;
	while (true)
	{
		//std::cout << "Orchestrator running: " << m_pAppData->m_colliders.size() << std::endl;
		m_pAppData->updateSimulation();

		i++;
		m_taskQueue.addTask([this, &i]() { std::cout << "Executing task " << i << " by thread " << std::this_thread::get_id() << std::endl; });
	}
}