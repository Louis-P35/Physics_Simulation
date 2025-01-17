// Includes from project
#include "main.hpp"
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

    QApplication app(argc, argv);
    MainWindow window(appData);
	
    window.show();

	std::cout << "Application started" << std::endl;
    initAfterOpenGl(appData, window);

	appData.m_pOpenGl3DWidget = window.m_pOpenGl3DWidgetClothSimulation;
	appData.initSimulation();

    
    // Stop the worker when the application quits
    QObject::connect(&app, &QApplication::aboutToQuit, [&]() {
		for (auto& pCloth : appData.m_pCloths)
		{
            if (pCloth)
            {
                pCloth->stopWorker();
            }
		}
        });

    return app.exec();
}


bool initAfterOpenGl(ApplicationData& appData, MainWindow& window)
{
    //Object3D test;
    //appData.m_monkey3D.loadFromObjFile("../models/test2/", "testCube.obj");

	//exit(0);

    //appData.m_monkey3D.loadFromObjFile("../models/Susanne/", "suzanne.obj");
    //appData.m_monkey3D.loadFromObjFile("../models/sphere/", "sphere.obj");

    //std::shared_ptr<Object3D> pCube = std::make_shared<Object3D>();
    //appData.m_cube3D.loadFromObjFile("../models/cube/", "cube.obj");
	//appData.m_pCube3DRenderer = window.m_pOpenGl3DWidgetClothSimulation->addObject(appData.m_cube3D);
	//appData.m_pCube3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	//appData.m_pCube3DRenderer->m_pPosRotScale->m_scale = { 0.3f, 0.3f, 0.3f };

    // Ground
    appData.m_ground3D.loadFromObjFile("../models/ground_2/", "ground.obj");
	appData.m_pGround3DRenderer = window.m_pOpenGl3DWidgetClothSimulation->addObject(appData.m_ground3D);
	appData.m_pGround3DRenderer->m_pPosRotScale->m_position = { 0.0f, 0.0f, 0.0f };
	appData.m_pGround3DRenderer->m_pPosRotScale->m_scale = { 1.0f, 1.0f, 1.0f };

	// Debug sphere
	appData.m_debugSphere3D.loadFromObjFile("../models/sphere/", "sphere.obj");
	appData.m_pDebugSphere3DRenderer = window.m_pOpenGl3DWidgetClothSimulation->addObject(appData.m_debugSphere3D);
	appData.m_pDebugSphere3DRenderer->m_pPosRotScale->m_position = { 0.0f, 2.0f, 0.0f };
	appData.m_pDebugSphere3DRenderer->m_pPosRotScale->m_scale = { 0.1f, 0.1f, 0.1f };

    return true;
}