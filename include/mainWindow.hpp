#pragma once

// Includes from 3rd party
#include <QMainWindow>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* pParent = nullptr);
};