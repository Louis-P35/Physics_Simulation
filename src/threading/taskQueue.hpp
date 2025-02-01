#pragma once

// Includes from STL
#include <deque>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <condition_variable>

/*
* TaskQueue
* A simple task queue that can be used to queue up tasks to be executed by a thread pool.
* The tasks are stored in a deque and can be added, retrieved and marked as done.
* The orchestrator thread can also wait until all tasks have been completed.
* The tasks are stored as std::function<void()> so they can be any callable object.
*/
class TaskQueue
{
private:
	std::deque<std::function<void()>> m_tasks;
	std::mutex m_mutex;
	std::atomic<int> m_taskCount = 0;
	std::condition_variable m_cv;

public:
	TaskQueue() = default;
	~TaskQueue() = default;

	void addTask(std::function<void()>&& taskCallback);
	void getTask(std::function<void()>& taskCallback);
	void markTaskAsDone();
	void waitUntilEmpty();
	void releaseAll(const size_t numberOfThreads);
	void clearTaskQueue();
};