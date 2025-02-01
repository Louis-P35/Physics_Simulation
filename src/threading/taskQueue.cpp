// Includes from project
#include "../src/threading/taskQueue.hpp"

// Includes from STL
#include <iostream>


/*
* Release all tasks from the queue
* 
* @return void
*/
void TaskQueue::releaseAll(const size_t numberOfThreads)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_tasks.clear();

	for (size_t i = 0; i < numberOfThreads; ++i)
	{
		m_tasks.push_back([]() {}); // Add dummy tasks to wake up all worker threads
	}

	m_cv.notify_all();
}


/*
* Clear the task queue
* 
* @return void
*/
void TaskQueue::clearTaskQueue()
{
	m_tasks.clear();
	m_taskCount = 0;
}


/*
* Add a task to the queue
* 
* @param taskCallback The task to add to the queue
* @return void
*/
void TaskQueue::addTask(std::function<void()>&& taskCallback)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_tasks.push_back(std::move(taskCallback));
	m_taskCount++;

	// Wake up all worker thread (if waiting in gettask())
	m_cv.notify_all(); // notify_one() may be better ? but in case...
}


/*
* Get a task from the queue, and remove it from the queue
* The task is moved into the taskCallback parameter
* 
* @param taskCallback The task to get from the queue
* @return void
*/
void TaskQueue::getTask(std::function<void()>& taskCallback)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	//std::cout << "Task count: " << m_taskCount << " " << m_tasks.size() << std::endl;

	// If there is tasks in the queue already, do not wait
	while (m_tasks.empty())
	{
		// wait() unlock the mutex and wait until the condition variable is notified
		// The lambda function is used to check if the task queue is empty
		// The mutex is locked again before returning
		m_cv.wait(lock, [this]() { return !m_tasks.empty(); });  // Sleep until a task is available
		// The notify_all() in addTask() will wake up only one worker thread waiting here
	}

	// Can happen, because the orchestrator thread can wake up more worker threads than there are tasks
	if (m_tasks.empty())
	{
		return;
	}

	taskCallback = std::move(m_tasks.front());
	m_tasks.pop_front();
}


/*
* Mark a task as done
* This is used to decrement the task count when a task has been completed
* 
* @return void
*/
void TaskQueue::markTaskAsDone()
{
	// Atomic operation so no mutex needed here
	m_taskCount--;
}


/*
* Wait until the task queue is empty
* This is used for the orchestrator thread to wait until all tasks have been completed
* 
* @return void
*/
void TaskQueue::waitUntilEmpty()
{
	//std::unique_lock<std::mutex> lock(m_mutex);
	//m_cv.wait(lock, [this]() { return m_taskCount == 0; });  // Sleep until all tasks are done
	// Atomic operation so no mutex needed here
	while (m_taskCount > 0)
	{
		// Try not consuming too much CPU by letting the scheduler do other things
		std::this_thread::yield();
	}
}