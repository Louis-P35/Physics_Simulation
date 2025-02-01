// Includes from project
#include "main.hpp"
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
// Gravité ?

// Taille tissus
// Résolution tissus
// Epaisseur tissus
// Poid du tissus
// Elasticité du tissus
// Texture & normal du tissu



int main(int argc, char** argv)
{
	ApplicationData appData;

    QApplication app(argc, argv);
    MainWindow window(appData);
	
    window.show();

	std::cout << "Application started" << std::endl;
    appData.initAfterOpenGl(window.m_pOpenGl3DWidgetClothSimulation);

    return app.exec();
}