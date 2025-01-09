// Includes from project
#include "main.hpp"
#include "mainWindow.hpp"
#include "applicationData.hpp"

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

// Taille tissus
// Résolution tissus
// Epaisseur tissus
// Poid du tissus
// Elasticité du tissus



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
	window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_monkey3D);
    window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_bench3D);
    window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_ground3D);

    return app.exec();
}


bool init(ApplicationData& appData)
{
    return true;
}


bool initAfterOpenGl(ApplicationData& appData)
{
    appData.m_monkey3D.loadFromObjFile("../models/Susanne/", "monkey.obj");
    appData.m_monkey3D.setPosition({ 0.0f, 4.0f, 0.0f });

    appData.m_bench3D.loadFromObjFile("../models/workBench_2/", "Table.obj");
    appData.m_bench3D.setPosition({ 0.0f, 0.0f, 0.0f });
    appData.m_bench3D.setScale({ 3.0f, 3.0f, 3.0f });

    // Ground
	appData.m_ground3D.loadFromObjFile("../models/ground/", "ground.obj");

    return true;
}