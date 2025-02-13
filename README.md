# Cloth Simulation Project
This repository contains a high-performance, real-time cloth simulation engine 
developed in C++ using Qt and OpenGL. The project showcases advanced techniques 
in physics simulation, collision detection, and multithreading—all implemented 
with modern C++.

## Overview
![Simulation Animation](imgs/Physicssimulation1.gif)

The simulation features multiple cloth objects falling and interacting in a 3D scene. 
The cloths, represented as grids of particles connected by spring forces, are updated 
using Euler integration. They collide with both static 3D objects, other cloths and 
themselves, achieving realistic behavior through sophisticated collision detection 
algorithms.

## Key Features

### Real-Time 3D Simulation:
A Qt window hosts an OpenGL view where the simulation unfolds in real time, using CPU
only for physics calculation. 

### Advanced Rendering:
3D objects are loaded from .obj files and rendered with realistic materials. Shaders 
implement normal mapping, tessellation, and bump mapping to achieve high-fidelity visuals.
To render a cloth, a mesh is computed from the cloth particles and a given thickness at 
each frame. Then, a tesselation and bump mapping shader is used increase the resolution
of the mesh of the cloth directly on the GPU.

### Cloth Simulation:
Cloths are modeled as grids of particles interconnected by springs. The simulation uses 
Euler integration to update particles positions over time.


### Robust And Fast Collision Detection:

#### Cloth-to-Object Collisions:
A 3D object is a list of triangles. The collision must be check between all the 
particles/triangles combinaisons.
To avoid an expensive O(n²) complexity, an octree structure is use and built from the 
3D object’s mesh. Each node of the octree contain a Axis Aligned Bounding Box (AABB) that
encloses the AABB of its children. The leaf nodes contain the triangles of the mesh.
This structure efficiently detects collisions between the cloth particles (modeled as moving 
spheres) and the mesh.

#### Cloth-to-Cloth Collisions: 
Each cloth particle is treated as a small sphere. The collision must be check between all pairs
of particles in the scene.
To avoid an expensive O(n²) complexity, two versions of a hashgrid algorithm are implemented:
A fast version using a 3D grid stored in a single vector. It use a lot of memory as there is a lot of 
empty cells.
```cpp
std::vector<std::shared_ptr<GridCell>> m_gridWrite;
std::vector<std::shared_ptr<GridCell>> m_gridRead;
```

```cpp
/*
* Get the index of the cell at the specified coordinates
* Because the grid is a 3D array stored in a 1D vector, we need to compute the index
* 
* @param x X coordinate of the cell
* @param y Y coordinate of the cell
* @param z Z coordinate of the cell
* @return size_t Index of the cell in the grid
*/
inline size_t StaticGridCollider::getCellIndex(const int x, const int y, const int z) const
{
	return x + y * m_gridWidth + z * m_gridWidthHeight;
}
```
To avoid looping over a lot of empty cells, a vector of pointer to the non-empty cells is stored and 
updated at each frame.


An alternative version using a hashmap for grid storage. It is slower but uses less memory. A hash is 
calculated from the grid cell coordinates and the particles are stored in a hashmap with the hash as key.
```cpp
std::unordered_map<size_t, std::shared_ptr<GridCell>> m_gridWrite;
std::unordered_map<size_t, std::shared_ptr<GridCell>> m_gridRead;
```

```cpp
/* 
* Helper function to compute a unique hash key from (x, y, z)
* 
* @param x X coordinate
* @param y Y coordinate
* @param z Z coordinate
* @return size_t Unique hash key
*/
inline size_t HashGridCollider::hashKey(const int x, const int y, const int z) const
{
    size_t h1 = std::hash<int>()(x);
    size_t h2 = std::hash<int>()(y);
    size_t h3 = std::hash<int>()(z);

	// Using the XOR bitwise operator to combine the hashes and minimize keys collisions
    return h1 ^ (h2 << 1) ^ (h3 << 2);
}
```

Each particle are stored in the grid cell corresponding to its position. The grid is updated at each frame. Then,
for each cell of the grid, the particles are checked for collisions with the particles in the same cell and 
the particles in the adjacent cells.
Double buffering: The grid is stored in two buffers. At each frame, the grid is updated in the write grid and 
the read grid is used for collision detection. At the end of the frame, the two grid buffers are swapped. This
double the memory usage but allows to avoid thread race.


### Multithreading for Performance:
A dedicated orchestrator (implemented as a singleton) manages a pool of worker threads. The orchestrator 
thread continuously subdivise the simulations in small tasks and fills a task queue with thoses simulation 
tasks (packaged as lambdas). The workers threads continuoulsy pull thoses tasks from the queue and execute
them in parallel.
```cpp
// Worker thread lambda function
// This just endlessley loops to gets and execute tasks from the task queue
auto workerThreadLambda = [this]()
{
	while (m_workerRunning)
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
```

```cpp
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
```



## Project setup

### If not already, install vcpkg
#### Clone repository
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
#### Build it
##### On windows
.\bootstrap-vcpkg.bat
##### On linux
./bootstrap-vcpkg.sh
### Install GLM using vcpkg
#### On windows
.\vcpkg.exe install glm
#### On linux
./vcpkg install glm
### Set up the environment variables
CMAKE_TOOLCHAIN_FILE C:\Users\louis\Documents\vcpkg\scripts\buildsystems\vcpkg.cmake
GLM_DIR C:\Users\louis\Documents\vcpkg\installed\x64-windows\share\glm
VCPKG_INCLUDE C:\Users\louis\Documents\vcpkg\installed\x64-windows\include
Restart your IDE


### QT setup
Setup Qt environment variables: 
CMAKE_PREFIX_PATH to (for exemple): C:/Qt/6.8.1/msvc2022_64
QT_QPA_PLATFORM_PLUGIN_PATH to (for example): C:\Qt\6.8.1\msvc2022_64\plugins\platforms
Restart IDE

### cmake
DeployQt.cmake is used to deploy the Qt libraries.