// Includes from project
#include "physicsWorker.hpp"



void PhysicsWorker::start(std::function<void()> updateCallback)
{
    m_running = true;
    m_thread = std::thread([this, updateCallback]()
        {
            while (m_running)
            {
                // Perform the physics update
                updateCallback();

                // Limit the loop rate (e.g., ~1ms updates)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
}

void PhysicsWorker::stop()
{
    m_running = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}