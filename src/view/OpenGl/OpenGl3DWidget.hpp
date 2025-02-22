#pragma once

// Include from project
#include "object3D.hpp"
#include "shader.hpp"
#include "objectRenderingInstance.hpp"

// Includes from 3rd party
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
//#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Includes from STL
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>
#include <functional>


/*
* OpenGl3DWidget class
* 
* This class is a custom QOpenGLWidget that is used to render 3D graphics
*/
class OpenGl3DWidget : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
    Q_OBJECT

public:
	std::vector<std::shared_ptr<ObjectRenderingInstance>> m_objectsToRenderList;
    Shader m_pShader;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QVector3D m_lightPosition;
    QVector3D m_cameraPosition;
    std::function<void()> m_InitDrawCallback;

public:
    explicit OpenGl3DWidget(std::function<void()> initDrawCallback, QWidget* pParent = nullptr);
    ~OpenGl3DWidget();

    void loadShaders();
    std::shared_ptr<ObjectRenderingInstance> initialyzeObject3D(Object3D& object3D);
    void updateObject3D(std::shared_ptr<ObjectRenderingInstance> pObjInst);
    void drawObject(std::shared_ptr<ObjectRenderingInstance> pObjRender);
    std::shared_ptr<ObjectRenderingInstance> addObject(Object3D& object3D);
	void removeAllObjects();
    void removeObject(std::shared_ptr<ObjectRenderingInstance> pObject);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
