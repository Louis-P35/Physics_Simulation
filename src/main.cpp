// Includes from project
#include "main.hpp"
#include "mainWindow.hpp"
#include "applicationData.hpp"
#include "physicsWorker.hpp"

// Includes from 3rd party
#include <QApplication>

// Includes from STL
#include <iostream>
#include <vector>
#include <string>
#include <tuple>


// Params :
// Obj plus ou moins glisssant
// Position, rotation, scale des objets
// Loading de l'objet au choix
// Gravité ?

// Taille tissus
// Résolution tissus
// Epaisseur tissus
// Poid du tissus
// Elasticité du tissus
// Texture & normal du tissu



int main(int argc, char** argv)
{
    // Init. TODO: Do the 3d object loading in a thread
	ApplicationData appData;
    init(appData);

    QApplication app(argc, argv);
    MainWindow window;
	
    window.show();

	std::cout << "Application started" << std::endl;
    initAfterOpenGl(appData);
	for (auto& object : appData.m_objects3D)
	{
        if (object)
        {
            window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(*object);
        }
	}
    window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_ground3D);
    window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_debugSphere3D);


    // Create the physics simulation worker
    PhysicsWorker physicsWorker;

    // Start the simulation in a separate thread
    physicsWorker.start([&]() {
        // Perform your physics updates
        appData.simulationUpdate();

        // Emit a signal to update GUI if needed
        //QApplication::postEvent(&window, new QEvent(QEvent::UpdateRequest));
        });
    
    // Stop the worker when the application quits
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
        physicsWorker.stop();
        });

    return app.exec();
}


bool init(ApplicationData& appData)
{
    return true;
}


bool initAfterOpenGl(ApplicationData& appData)
{
    //Object3D test;
    //appData.m_monkey3D.loadFromObjFile("../models/test2/", "testCube.obj");

	//exit(0);

    //appData.m_monkey3D.loadFromObjFile("../models/Susanne/", "suzanne.obj");
    //appData.m_monkey3D.loadFromObjFile("../models/sphere/", "sphere.obj");

    std::unique_ptr<Object3D> pCube = std::make_unique<Object3D>();
    pCube->loadFromObjFile("../models/cube/", "cube.obj");
    pCube->setPosition({ 0.0f, 2.0f, 0.0f });
    pCube->setScale({ 0.3f, 0.3f, 0.3f });
    appData.m_objects3D.push_back(std::move(pCube));

    //appData.m_bench3D.loadFromObjFile("../models/workBench_2/", "Table.obj");
    //appData.m_bench3D.loadFromObjFile("../models/cube/", "cube.obj");
    //appData.m_bench3D.loadFromObjFile("../models/workBench_2/", "Table_b.obj");
    //appData.m_bench3D.setPosition({ 0.0f, 0.0f, 0.0f });
    //appData.m_bench3D.setScale({ 3.0f, 3.0f, 3.0f });

    // Ground
    appData.m_ground3D.loadFromObjFile("../models/ground_2/", "ground.obj");

	// Debug sphere
	appData.m_debugSphere3D.loadFromObjFile("../models/sphere/", "sphere.obj");
    appData.m_debugSphere3D.setScale({ 0.1f, 0.1f, 0.1f });
    appData.m_debugSphere3D.setPosition({ 0.0f, 2.0f, 0.0f });

    return true;
}