// Includes from project
#include "mainWindow.hpp"
#include "../src/view/Qt/clothWidget.hpp"

// Includes from 3rd party
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTabWidget>
#include <QSplitter>

MainWindow::MainWindow(QWidget* pParent) : QMainWindow(pParent)
{
    // Set title
	setWindowTitle("Physics simulation");

    // Set a minimum size for the window
    setMinimumSize(m_widowInitialSizeX, m_widowInitialSizeY);

    // Menubar
    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Exit", this, &QMainWindow::close);

    // Statusbar
    statusBar()->showMessage("Ready");

    // Create a QTabWidget
    QTabWidget* pTabWidget = new QTabWidget(this);

	// Create a tab for Cloth simulation
    m_pOpenGl3DWidgetClothSimulation = new OpenGl3DWidget(this);
	this->createTab("Cloth simulation", m_pOpenGl3DWidgetClothSimulation, pTabWidget);
    

    // Create second tab
	this->createTab("Fluid simulation", nullptr, pTabWidget);


    // Set the QTabWidget as the central widget of the main window
    setCentralWidget(pTabWidget);
}


/*
* Create a tab with a given name and add it to the QTabWidget
* A vertical splitter is dividing the area into two panes (control & rendering)
* 
* @param tabName Name of the tab
* @param pGlWidget OpenGl3DWidget to add to the tab
* @param pTabWidget QTabWidget to add the tab to
* @return QWidget* Pointer to the created tab
*/
QWidget* MainWindow::createTab(const std::string& tabName, OpenGl3DWidget* pGlWidget, QTabWidget* pTabWidget)
{
	// Create a tab
	QWidget* pTab = new QWidget();

    // Create a vertical splitter to divide the area horizontally
    QSplitter* pSplitter = new QSplitter(Qt::Horizontal, pTab);

    // Left widget: placeholder for future controls or content
    QWidget* pLeftWidget = new QWidget(pSplitter);
    QVBoxLayout* pLeftLayout = new QVBoxLayout(pLeftWidget);
	ClothWidget* pClothWidget = new ClothWidget(pLeftWidget);
    pLeftLayout->addWidget(pClothWidget);

    // Right widget: for OpenGl rendering area
    QWidget* pRightWidget = new QWidget(pSplitter);
    QVBoxLayout* pRightLayout = new QVBoxLayout(pRightWidget);
    
    if (pGlWidget)
    {
        pGlWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        pRightLayout->addWidget(pGlWidget);
    }

    // Add left and right widgets to the splitter
    pSplitter->addWidget(pLeftWidget);
    pSplitter->addWidget(pRightWidget);

    // Set initial stretch factors for the splitter to approximate 1/3 left, 2/3 right
    pSplitter->setStretchFactor(0, 1); // left widget
    pSplitter->setStretchFactor(1, 2); // right widget

	// Set initial sizes for the splitter
    QList<int> sizes;
    sizes << (m_widowInitialSizeX / 2) << m_widowInitialSizeX;
    pSplitter->setSizes(sizes);

    // Set the splitter as the layout for the tab
    QVBoxLayout* pTabLayout = new QVBoxLayout(pTab);
    pTabLayout->addWidget(pSplitter);
    pTabLayout->setContentsMargins(0, 0, 0, 0);
    pTabLayout->setSpacing(0);

	// Add tabs to the QTabWidget
    pTabWidget->addTab(pTab, tabName.c_str());

	return pTab;
}