#pragma once

// Includes from 3rd party
#include <QMainWindow>
#include <QWidget>


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* pParent = nullptr);

private:
	QWidget* createTab(const std::string& tabName, QTabWidget* pTabWidget);
};