#pragma once

// Include from project
#include "../src/view/OpenGl/OpenGl3DWidget.hpp"

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

public:
	explicit MainWindow(QWidget* pParent = nullptr);

private:
	QWidget* createTab(const std::string& tabName, QTabWidget* pTabWidget);
};