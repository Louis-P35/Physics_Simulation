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


Orchestrator::~Orchestrator()
{
	// Stop the orchestrator thread
	stop();

	// Join the worker threads
	for (auto& workerThread : m_workerThreads)
	{
		if (workerThread.joinable())
		{
			workerThread.join();
		}
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
	static Orchestrator instance(1);
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
	// Initialize the last update time
	m_lastUpdateTime = std::chrono::steady_clock::now();

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

/*
* Stop the orchestrator by joining the orchestrator thread
* 
* @return void
*/
void Orchestrator::stop()
{
	// Stop the orchestrator thread
	if (m_orchestratorThread.joinable())
	{
		m_orchestratorThread.join();
	}
}


/*
* Run the orchestrator
* This is the main simulation thread
* Dispatches tasks to the worker threads via the task queue
* 
* @return void
*/
void Orchestrator::runOrchestrator()
{
	const size_t cellsBatchSize = 50;
	const int resxBatchSize = 5;

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

	double sommeDt = 0.0;
	double avg = 0.0;
	int count = 0;

	while (true)
	{
		// Calculate the time elapsed since the last update
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> deltaTime = currentTime - m_lastUpdateTime;
		m_lastUpdateTime = currentTime;

		// Convert deltaTime to seconds
		double elapsedTimeInSeconds = static_cast<double>(deltaTime.count());

		// Calculate the average time step for debugging performance
		sommeDt += elapsedTimeInSeconds;
		count++;
		avg = sommeDt / static_cast<double>(count);

		// Clamp the time step to avoid huge time steps
		// This is a simple way to avoid instability in the simulation
		if (elapsedTimeInSeconds > 0.005)
		{
			elapsedTimeInSeconds = 0.005;
		}

		// First, update all the cloths' particles and the collisions with static colliders

		auto t1 = std::chrono::steady_clock::now();

		// Update all the cloths' particles and the collisions with static colliders
		// Add the particles to the hash grid collider
		for (auto& pCloth : m_pAppData->m_pCloths.m_pCloths)
		{
			if (pCloth)
			{
				// Create tasks to update the particles of the cloth
				for (int i = 0; i < pCloth->m_resX; i += resxBatchSize)
				{
					int startResX = i;
					int endResX = std::min(startResX + resxBatchSize, pCloth->m_resX);
				
					m_taskQueue.addTask(
						[this , pCloth, elapsedTimeInSeconds, startResX, endResX]() {
							// Update the simulation
							pCloth->updateParticles(
								elapsedTimeInSeconds, 
								startResX, endResX, 
								m_pAppData->m_colliders, 
								m_pAppData->m_pGridCollider
							);
						});
					/*pCloth->updateParticles(
						elapsedTimeInSeconds,
						startResX, endResX,
						m_pAppData->m_colliders,
						m_pAppData->m_pGridCollider
					);*/
				}
			}
		}

		// Wait until all clothes' particles have been updated before setting their previous position and velocity
		m_taskQueue.waitUntilEmpty();

		// Calculate the average time step for debugging performance
		auto t2 = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt1 = t2 - t1;
		static double sommeDt1 = 0.0;
		sommeDt1 += static_cast<double>(dt1.count());
		double avg1 = sommeDt1 / static_cast<double>(count);

		// Create tasks to update the previous position and velocity of the particles
		for (auto& pCloth : m_pAppData->m_pCloths.m_pCloths)
		{
			if (pCloth)
			{
				for (int i = 0; i < pCloth->m_resX; i += resxBatchSize)
				{
					int startResX = i;
					int endResX = std::min(startResX + resxBatchSize, pCloth->m_resX);

					/*m_taskQueue.addTask(
						[this, pCloth, startResX, endResX]() {
							// Update the previous position and velocity
							pCloth->updatePreviousPositionAndVelocity(startResX, endResX);
						});*/
					pCloth->updatePreviousPositionAndVelocity(startResX, endResX);
				}
				// Add the particle's new position to the grid collider
				pCloth->updateGridCollider(m_pAppData->m_pGridCollider);
			}
		}

		// Wait until all clothes' to be ready to check collisions
		m_taskQueue.waitUntilEmpty();

		// Calculate the average time step for debugging performance
		auto t3 = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt2 = t3 - t2;
		static double sommeDt2 = 0.0;
		sommeDt2 += static_cast<double>(dt2.count());
		double avg2 = sommeDt2 / static_cast<double>(count);

		// From here, all particles' position and previousPosition are the same
		// So we can resolve the collisions between the particles using the particles' previousPosition

		// Then, resolve the collisions between the cloths
		if (m_pAppData->m_pGridCollider)
		{
			std::vector<std::vector<GridCell*>> CellsFromReadGrid;
			size_t cellsCount = 0;

			// Loop over all the non-empty (existing) grid cells
			//for (auto& cell : m_pAppData->m_pGridCollider->m_gridRead) // HashGrid
			for (auto& pCell : m_pAppData->m_pGridCollider->m_listOfPointerToNonEmptyCellsRead)
			{
				// Create batches of cells to allow parallelism
				if (cellsCount > cellsBatchSize || CellsFromReadGrid.size() == 0)
				{
					CellsFromReadGrid.push_back(std::vector<GridCell*>());
					cellsCount = 0;
				}
				cellsCount++;

				//CellsFromReadGrid.back().push_back(&cell.second); // HashGrid
				CellsFromReadGrid.back().push_back(pCell);
			}

			// Create tasks to resolve the collisions between the particles
			for (auto& cellsBatch : CellsFromReadGrid)
			{
				m_taskQueue.addTask(
					[this, cellsBatch]() { // TODO : cellsBatch can be a shared_ptr to avoid copying ? (can not be a ref)
						m_pAppData->updateCollisions(cellsBatch);
					});
				//m_pAppData->updateCollisions(cellsBatch);
			}
		}

		// Wait until all collisions to be reselved before setting their previous position and velocity
		m_taskQueue.waitUntilEmpty();

		// Calculate the average time step for debugging performance
		auto t4 = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt3 = t4 - t3;
		static double sommeDt3 = 0.0;
		sommeDt3 += static_cast<double>(dt3.count());
		double avg3 = sommeDt3 / static_cast<double>(count);

		// Update previousPositions
		for (auto& pCloth : m_pAppData->m_pCloths.m_pCloths)
		{
			if (pCloth)
			{
				for (int i = 0; i < pCloth->m_resX; i += resxBatchSize)
				{
					int startResX = i;
					int endResX = std::min(startResX + resxBatchSize, pCloth->m_resX);

					m_taskQueue.addTask(
						[this, pCloth, startResX, endResX]() {
							// Update the previous position and velocity
							pCloth->updatePreviousPositionAndVelocity(startResX, endResX);
						});
					//pCloth->updatePreviousPositionAndVelocity(startResX, endResX);
				}
			}
		}

		// Clear the read grid
		for (size_t i = 0; i < m_pAppData->m_pGridCollider->m_listOfPointerToNonEmptyCellsRead.size(); i += cellsBatchSize)
		{
			size_t start = i;
			size_t end = std::min(start + cellsBatchSize, m_pAppData->m_pGridCollider->m_listOfPointerToNonEmptyCellsRead.size());
			m_taskQueue.addTask(
				[this, start, end]() {
					m_pAppData->m_pGridCollider->clearGridParallelized(start, end);
				});
			//m_pAppData->m_pGridCollider->clearGridParallelized(start, end);
		}

		// Wait until all clothes' to be ready for the next update
		m_taskQueue.waitUntilEmpty();

		//std::cout << "MEMORY : " << (m_pAppData->m_pGridCollider->getMemorySize() / (1024 * 1024)) << " Mo" << std::endl;


		// Clear the list of pointers to non-empty cells of the read grid
		m_pAppData->m_pGridCollider->m_listOfPointerToNonEmptyCellsRead.clear();
		// Swap the read and write grids (fast if m_listOfPointerToNonEmptyCellsRead is already cleared)
		m_pAppData->m_pGridCollider->swap();
		// Cost 0.0019s in monothread debug

		// Calculate the average time step for debugging performance
		auto t5 = std::chrono::steady_clock::now();
		std::chrono::duration<float> dt4 = t5 - t4;
		static double sommeDt4 = 0.0;
		sommeDt4 += static_cast<double>(dt4.count());
		double avg4 = sommeDt4 / static_cast<double>(count);
		avg4 = avg4;
	}
}