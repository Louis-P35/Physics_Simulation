#pragma once

// Include from project
#include "object3D.hpp"

// Includes from 3rd party
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Includes from STL
#include <vector>


struct ObjectRenderingInstance
{
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    GLuint m_shaderProgram;

    std::vector<VBOVertex> m_verticesData;
};


/*
* OpenGl3DWidget class
* 
* This class is a custom QOpenGLWidget that is used to render 3D graphics
*/
class OpenGl3DWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
	std::vector<std::unique_ptr<ObjectRenderingInstance>> m_objectsToRenderList;

public:
    explicit OpenGl3DWidget(QWidget* pParent = nullptr);
    ~OpenGl3DWidget();

	void initialyzeObject3D(Object3D& object3D);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
