// Includes from project
#include "main.hpp"
#include "mainWindow.hpp"
// Includes from project
#include "main.hpp"
#include "view/OpenGl/object3D.hpp" 

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
    init();

    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}


bool init()
{
	Object3D monkey3D(Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(1, 1, 1));

	monkey3D.loadFromObjFile("../models/monkey.obj");

    return true;
}