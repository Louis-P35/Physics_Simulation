#pragma once

// Includes from STL
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <functional>
#include <atomic>


/*
* class DynamicBarrier
*
* Synchronize threads at a barrier
* C++20 implement this class with std::barrier, but we need a dynamic m_threshold
* which is not possible with std::barrier
*/
class DynamicBarrier
{
private:
	size_t m_count;
	size_t m_threshold;
	std::mutex m_mutex;
	std::condition_variable m_cv;

public:
    DynamicBarrier() : m_count(0), m_threshold(0) {}

    /*
	* setThreshold function
    * 
	* Set the threshold of the barrier
    * 
	* @param threshold The new threshold
	* @return void
    */
    void setThreshold(size_t threshold)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_threshold = threshold;
        m_cv.notify_all();
    }

    void increaseThreshold()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_threshold++;
        m_cv.notify_all();
    }

    void decreaseThreshold()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        if (m_threshold > 0)
        {
            m_threshold--;
        }

        if (m_count >= m_threshold)
        {
            m_count = 0;
        }

        m_cv.notify_all();
    }

    /*
	* arriveAndWait function
    * 
	* Synchronize threads at a barrier
    * 
	* @return void
    */
    void arriveAndWait(std::function<void()> onLastThreadCallback)
    {
		// A thread that arrives here will lock the mutex
        std::unique_lock<std::mutex> lock(m_mutex);

		// Then it will increment the count
        m_count++;

        if (m_count >= m_threshold)
        {
            // The last thread triggers the callback
            if (onLastThreadCallback)
            {
                onLastThreadCallback();
            }

			// If the count is enough, the count is reinitialized and all the threads are notified
            m_count = 0;  // Reinitialyze for the next cycle
            m_cv.notify_all();
        }
        else
        {
			// If the count is not enough, the mutex is unlocked and the thread waits
			// until the count is zero (it will be zero when the last thread arrives)
			// This wait() function wait a notification from the notify_all() function
            m_cv.wait(lock, [this]() { return m_count == 0; });
        }
    }
};