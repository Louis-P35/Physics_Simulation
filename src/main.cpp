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



int main(int argc, char** argv)
{
    // Init. TODO: Do the 3d object loading in a thread
	ApplicationData appData;
    init(appData.m_monkey3D);

    QApplication app(argc, argv);
    MainWindow window;
	
    window.show();

	std::cout << "Application started" << std::endl;
	window.m_pOpenGl3DWidgetClothSimulation->initialyzeObject3D(appData.m_monkey3D);

    return app.exec();
}


bool init(Object3D& obj)
{
    obj.loadFromObjFile("../models/monkey.obj");

    return true;
}