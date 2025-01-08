#pragma once

// Includes from 3rd party
#include <QOpenGLWidget>
#include <QOpenGLFunctions>

/*
* OpenGl3DWidget class
* 
* This class is a custom QOpenGLWidget that is used to render 3D graphics
*/
class OpenGl3DWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OpenGl3DWidget(QWidget* pParent = nullptr);
    ~OpenGl3DWidget();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
