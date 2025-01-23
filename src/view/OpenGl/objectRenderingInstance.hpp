#pragma once
// Include from project
#include "object3D.hpp"
#include "shader.hpp"

// Includes from 3rd party
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Includes from STL
#include <vector>
#include <memory>
#include <mutex>
#include <iostream>

/*
* Struct for OpenGl rendering
*
* m_verticesData is a list of VBOVertex that represent the vertices of the object
*/
class ObjectRenderingInstance
{
public:
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    GLuint m_shaderProgram = 0;

    std::vector<VBOVertex> m_verticesData;

    std::shared_ptr<ObjectHandle> m_pPosRotScale;

    std::shared_ptr<QOpenGLTexture> m_pColorTexture;
    std::shared_ptr<QOpenGLTexture> m_pNormalTexture;
    std::shared_ptr<QOpenGLTexture> m_pBumpTexture;

    bool m_isStatic = true;
    mutable std::mutex m_mutex;

public:
    ObjectRenderingInstance() {};
    ~ObjectRenderingInstance()
    {
        std::cout << "ObjectRenderingInstance destroyed " << m_verticesData.size() << std::endl;
        if (m_vbo.isCreated())
        {
            m_vbo.destroy();
        }
        if (m_vao.isCreated())
        {
            m_vao.destroy();
        }
    };
};