#pragma once

// Includes from 3rd party
#include <QWidget>
#include <QSlider>
#include <QLabel>



class ClothWidget : public QWidget
{
	Q_OBJECT

private:
	QSlider* m_pSliderSize;
	QSlider* m_pSliderResolution;
	QLabel* m_pSliderSizeValueLabel;
	QLabel* m_pSliderResolutionValueLabel;

public:
	explicit ClothWidget(QWidget* pParent = nullptr);

private:
	void setupUI();

private slots:
	void OnSizeChange(int value);
	void OnResolutionChange(int value);

};