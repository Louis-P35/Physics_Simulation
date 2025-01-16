#pragma once

// Include from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"
#include "../src/view/Qt/clothWidget.hpp"
#include "applicationData.hpp"

// Includes from 3rd party
#include <QMainWindow>
#include <QWidget>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	OpenGl3DWidget* m_pOpenGl3DWidgetClothSimulation;

private:
	int m_widowInitialSizeX = 800;
	int m_widowInitialSizeY = 600;

	ApplicationData& m_appData;

public:
	explicit MainWindow(ApplicationData& appData, QWidget* pParent = nullptr);
	ClothWidget* m_pClothWidget;

private:
	QWidget* createTab(const std::string& tabName, OpenGl3DWidget* pGlWidget, QWidget* pLeftPannel, QTabWidget* pTabWidget);

private slots:
	void onResetClicked();
};