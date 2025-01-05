// Includes from project
#include "mainWindow.hpp"
#include "StringUtils.hpp"

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
	setWindowTitle("Qt6 Template Application");

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

    // Create a vertical layout for the central widget
    QVBoxLayout* pMainLayout = new QVBoxLayout(pCentralWidget);

    // First field
    QHBoxLayout* pFirstFieldLayout = new QHBoxLayout;
    QLabel* pFirstLabel = new QLabel("Enter the string to look into:", this);
    QLineEdit* pFirstLineEdit = new QLineEdit(this);
    pFirstFieldLayout->addWidget(pFirstLabel);
    pFirstFieldLayout->addWidget(pFirstLineEdit);

    // Second field
    QHBoxLayout* pSecondFieldLayout = new QHBoxLayout;
    QLabel* pSecondLabel = new QLabel("Enter the string to look for (with *):", this);
    QLineEdit* pSecondLineEdit = new QLineEdit(this);
    pSecondFieldLayout->addWidget(pSecondLabel);
    pSecondFieldLayout->addWidget(pSecondLineEdit);

    // Add the field layouts to the main layout
    pMainLayout->addLayout(pFirstFieldLayout);
    pMainLayout->addLayout(pSecondFieldLayout);

	// Add a button to check the pattern
	QPushButton* pButton = new QPushButton("Check", this);
	// Add the button to the main layout
	pMainLayout->addWidget(pButton);

	// Connect the button to the slot, define it on the fly (lambda)
	connect(pButton, &QPushButton::clicked, [pFirstLineEdit, pSecondLineEdit]()
		{
			const std::string str = pFirstLineEdit->text().toStdString();
			const std::string pattern = pSecondLineEdit->text().toStdString();
			const bool isMatch = StringUtils::match(str, pattern);
			if (isMatch)
			{
				QMessageBox::information(nullptr, "Match", "The pattern matches the string");
			}
			else
			{
				QMessageBox::information(nullptr, "No match", "The pattern does not match the string");
			}
		});
}