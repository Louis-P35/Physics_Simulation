#pragma once

// Includes from project

// Includes from 3rd party

// Includes from STL
#include <thread>
#include <atomic>
#include <functional>

class PhysicsWorker
{
private:
	std::thread m_thread;
	std::atomic<bool> m_running;

public:
	PhysicsWorker() : m_running(false) {};
	~PhysicsWorker() {
		stop();
	};

	void start(std::function<void()> updateCallback);
	void stop();
	void run();
};