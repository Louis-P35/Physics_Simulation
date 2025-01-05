// Includes from project
#include "mainWindow.hpp"

// Includes from 3rd party
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* pParent) : QMainWindow(pParent)
{
    // Set title
	setWindowTitle("Physics simulation");

    // Set a minimum size for the window
    setMinimumSize(800, 600);

    // Menubar
    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Exit", this, &QMainWindow::close);

    // Statusbar
    statusBar()->showMessage("Ready");

	// Central widget
    QWidget* pCentralWidget = new QWidget(this);
    setCentralWidget(pCentralWidget);
}