#pragma once

// Include from project
#include "object3D.hpp"
#include "shader.hpp"

// Includes from 3rd party
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Includes from STL
#include <vector>
#include <memory>


/*
* Struct for OpenGl rendering
* 
* m_verticesData is a list of VBOVertex that represent the vertices of the object
*/
struct ObjectRenderingInstance
{
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    GLuint m_shaderProgram;

    std::vector<VBOVertex> m_verticesData;

	std::shared_ptr<ObjectHandle> m_pPosRotScale;

    std::shared_ptr<QOpenGLTexture> m_pColorTexture;
    std::shared_ptr<QOpenGLTexture> m_pNormalTexture;

    bool m_isStatic = true;
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
	std::vector<std::shared_ptr<ObjectRenderingInstance>> m_objectsToRenderList;
    Shader m_pShader;
    QMatrix4x4 m_view;
    QMatrix4x4 m_projection;
    QVector3D m_lightPosition;
    QVector3D m_cameraPosition;

public:
    explicit OpenGl3DWidget(QWidget* pParent = nullptr);
    ~OpenGl3DWidget();

    void loadShaders();
    std::shared_ptr<ObjectRenderingInstance> initialyzeObject3D(Object3D& object3D);
    void updateObject3D(std::shared_ptr<ObjectRenderingInstance> pObjInst);
    void drawObject(std::shared_ptr<ObjectRenderingInstance> pObjRender);
    std::shared_ptr<ObjectRenderingInstance> addObject(Object3D& object3D);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
};
