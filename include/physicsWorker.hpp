#pragma once

// Includes from project

// Includes from 3rd party

// Includes from STL
#include <thread>
#include <atomic>
#include <functional>

/*
* PhysicsWorker class
* 
* This class is used to run the physics simulation in a separate thread
*/
class PhysicsWorker
{
private:
	std::thread m_thread;
	std::atomic<bool> m_running;
	std::atomic<bool> m_killReady;

public:
	PhysicsWorker() : m_running(false), m_killReady(true) {};
	~PhysicsWorker();

	void startWorker(std::function<void()> updateCallback);
	void stopThread();
	void stopRunning();
};