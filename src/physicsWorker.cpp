// Includes from project
#include "physicsWorker.hpp"


PhysicsWorker::~PhysicsWorker()
{
    stopWorker();
}


/*
* Start the physics simulation
* 
* This function starts the physics simulation in a separate thread
* 
* @param updateCallback Callback function that is called to update the physics simulation
* @return void
*/
void PhysicsWorker::startWorker(std::function<void()> updateCallback)
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

/*
* Stop the physics simulation
*/
void PhysicsWorker::stopWorker()
{
    m_running = false;
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}