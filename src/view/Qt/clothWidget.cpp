// Include from project
#include "clothWidget.hpp"

// Includes from 3rd party
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <Qstring>


ClothWidget::ClothWidget(QWidget* pParent) : QWidget(pParent)
{
    setupUI();
}

/*
* Setup the UI of the cloth creation widget
* 
* @return void
*/
void ClothWidget::setupUI()
{
    QVBoxLayout* pLayout = new QVBoxLayout(this);

    // Size slider
    QLabel* pLabelSize = new QLabel("Cloth size", this);
    m_pSliderSize = new QSlider(Qt::Horizontal, this);
    m_pSliderSize->setRange(10, 1000);
	int defaultSize = 30;
    m_pSliderSize->setValue(defaultSize); // Default value
    m_pSliderSizeValueLabel = new QLabel(QString::number(defaultSize), this);

    QHBoxLayout* pSizeLayout = new QHBoxLayout(); // Horizontal layout for size slider
    pSizeLayout->addWidget(pLabelSize);
    pSizeLayout->addWidget(m_pSliderSize);
    pSizeLayout->addWidget(m_pSliderSizeValueLabel);

    // Resolution slider
    QLabel* pLabelRes = new QLabel("Cloth resolution", this);
    m_pSliderResolution = new QSlider(Qt::Horizontal, this);
    m_pSliderResolution->setRange(10, 1000);
    int defaultRes = 30;
    m_pSliderResolution->setValue(defaultRes); // Default value
    m_pSliderResolutionValueLabel = new QLabel(QString::number(defaultRes), this);

    QHBoxLayout* pResolutionLayout = new QHBoxLayout(); // Horizontal layout for resolution slider
    pResolutionLayout->addWidget(pLabelRes);
    pResolutionLayout->addWidget(m_pSliderResolution);
    pResolutionLayout->addWidget(m_pSliderResolutionValueLabel);

    // Add the grouped layouts to the main vertical layout
    pLayout->addLayout(pSizeLayout);
    pLayout->addLayout(pResolutionLayout);

    // Add spacing or stretch as needed
    pLayout->addStretch();


    connect(m_pSliderSize, &QSlider::valueChanged, this, &ClothWidget::OnSizeChange);
    connect(m_pSliderResolution, &QSlider::valueChanged, this, &ClothWidget::OnResolutionChange);
}

/*
* Slot for size slider value change
* 
* @param value New value of the slider
* @return void
*/
void ClothWidget::OnSizeChange(int value)
{
    m_pSliderSizeValueLabel->setText(QString::number(value));
}

/*
* Slot for resolution slider value change
* 
* @param value New value of the slider
* @return void
*/
void ClothWidget::OnResolutionChange(int value)
{
    m_pSliderResolutionValueLabel->setText(QString::number(value));
}
