// Includes from project
#include "OpenGl3DWidget.hpp"

// Includes from 3rd party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QSurfaceFormat>
#include <QMatrix4x4>

// Includes from STL
#include <cmath>
#include <iostream>

/*
* OpenGl3DWidget constructor
*/
OpenGl3DWidget::OpenGl3DWidget(QWidget* pParent) : QOpenGLWidget(pParent)
{
    // Request a compatibility profile for fixed-function pipeline support
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setVersion(2, 1);  // Requesting an older version for compatibility
    setFormat(format);
}

/*
* OpenGl3DWidget destructor
*/
OpenGl3DWidget::~OpenGl3DWidget()
{
}

/*
* Initialize OpenGL
* 
* This function is called once when the OpenGL context is created
* 
* @return void
*/
void OpenGl3DWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Set clear color and enable depth testing
    glClearColor(0.229f, 0.508f, 0.622f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Setup basic lighting (optional)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	std::cout << "OpenGL initialized" << std::endl;

    loadShaders();
}


/*
* Resize OpenGL
* 
* This function is called whenever the widget is resized
* 
* @param w New width
* @param h New height
* @return void
*/
void OpenGl3DWidget::resizeGL(int w, int h)
{
    if (h == 0)
    {
        h = 1;
    }

    glViewport(0, 0, w, h);

    float aspect = static_cast<float>(w) / static_cast<float>(h);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*
* Paint OpenGL
* 
* This function is called whenever the widget needs to be repainted
* This is where the rendering code goes
* 
* @return void
*/
void OpenGl3DWidget::paintGL()
{
    //QVector3D lightPosition(0.0f, 8.0f, 8.0f);
    QVector3D lightPosition(8.0f, 8.0f, 8.0f);
    //QVector3D cameraPosition(0.0f, 8.0f, 8.0f);
    QVector3D cameraPosition(3.0f, 6.0f, 3.0f);
    QVector3D cameraLookAt(0.0f, 4.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all 3d objects
	for (auto& renderer : m_objectsToRenderList)
	{
        QMatrix4x4 model;
        //static float angle = 0.0f;
        model.setToIdentity();
		// Apply the translation
        model.translate(renderer->m_position[0], renderer->m_position[1], renderer->m_position[2]);
		// Apply the scale
        model.scale(renderer->m_scale[0], renderer->m_scale[1], renderer->m_scale[2]);
        //model.rotate(angle, 1.0f, 1.0f, 0.0f);
        //angle += 1.0f;

        QMatrix4x4 view;
        view.lookAt(
            cameraPosition,             // Camera position
            cameraLookAt,               // Point looked by the camera
            QVector3D(0.0f, 1.0f, 0.0f) // Up vector
        );

        QMatrix4x4 projection;
        projection.perspective(45.0f, float(width()) / height(), 0.1f, 100.0f);

        // Activate the shader and define the uniformes
        m_pShader.m_shaderProgram.bind();

		// Set the uniforms
        // matrix
        m_pShader.m_shaderProgram.setUniformValue("model", model);
        m_pShader.m_shaderProgram.setUniformValue("view", view);
        m_pShader.m_shaderProgram.setUniformValue("projection", projection);
        // Light pos and camera pos
        m_pShader.m_shaderProgram.setUniformValue("lightPos", lightPosition);
        m_pShader.m_shaderProgram.setUniformValue("viewPos", cameraPosition);

        // Set textures
        // color
        bool colorTextureAvailable = (renderer->m_pColorTexture != nullptr);
        m_pShader.m_shaderProgram.setUniformValue("useColorTexture", colorTextureAvailable);
        if (colorTextureAvailable)
        {
            glActiveTexture(GL_TEXTURE0);  // Select texture 0 unit
            if (renderer->m_pColorTexture)
            {
                renderer->m_pColorTexture->bind();
            }
            m_pShader.m_shaderProgram.setUniformValue("colorTexture", 0);  // Link sampler to texture unit 0
        }

        // normal
        bool normalTextureAvailable = (renderer->m_pNormalTexture != nullptr);
        m_pShader.m_shaderProgram.setUniformValue("useNormalTexture", normalTextureAvailable);
        if (normalTextureAvailable)
        {
            glActiveTexture(GL_TEXTURE1);  // Select texture 1 unit
            if (renderer->m_pNormalTexture)
            {
                renderer->m_pNormalTexture->bind();
            }
            m_pShader.m_shaderProgram.setUniformValue("normalTexture", 1);  // Link sampler to texture unit 1
        }

        renderer->m_vao.bind();
		glDrawArrays(GL_TRIANGLES, 0, GLsizei(renderer->m_verticesData.size()));
        renderer->m_vao.release();

        m_pShader.m_shaderProgram.release();
	}

    // Trigger a repaint for continuous rotation (TODO: use timer for fps handling)
    update();
}



/*
* Initialize a 3D object renderer (VAo & VBO) from an Object3D
* and move it to the list of objects to render
* 
* @param object3D Object3D to initialize
* @return void
*/
void OpenGl3DWidget::initialyzeObject3D(Object3D& object3D)
{
    std::cout << "Initialize VAO & VBO" << std::endl;

	std::unique_ptr<ObjectRenderingInstance> objInst = std::make_unique<ObjectRenderingInstance>();
	objInst->m_verticesData = object3D.computeVBOVerticesData();

    // Initialize VAO
    objInst->m_vao.create();
    objInst->m_vao.bind();

    // Initialize VBO
    objInst->m_vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    objInst->m_vbo.create();
    objInst->m_vbo.bind();
    objInst->m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    objInst->m_vbo.allocate(objInst->m_verticesData.data(), int(objInst->m_verticesData.size() * sizeof(VBOVertex)));

    // The shader use:
    // - location 0 for position
    // - location 1 for normal
    // - location 2 for uv
    GLint posLoc = 0;
    GLint normLoc = 1;
    GLint uvLoc = 2;
    GLint tangentLoc = 3;
    GLint bitangentLoc = 4;

    // Position offset
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), reinterpret_cast<void*>(offsetof(VBOVertex, position)));

    // Normal offset
    glEnableVertexAttribArray(normLoc);
    glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), reinterpret_cast<void*>(offsetof(VBOVertex, normal)));

    // UVs offset
    glEnableVertexAttribArray(uvLoc);
    glVertexAttribPointer(uvLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), reinterpret_cast<void*>(offsetof(VBOVertex, uv)));

    // Tangent offset
    glEnableVertexAttribArray(tangentLoc);
    glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), reinterpret_cast<void*>(offsetof(VBOVertex, tangent)));

    // Bitangent offset
    glEnableVertexAttribArray(bitangentLoc);
    glVertexAttribPointer(bitangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VBOVertex), reinterpret_cast<void*>(offsetof(VBOVertex, bitangent)));

    objInst->m_vao.release();
    objInst->m_vbo.release();

    // Copy the position, rotation and scale
    objInst->m_position = object3D.m_position;
    objInst->m_rotation = object3D.m_rotation;
    objInst->m_scale = object3D.m_scale;

	// Copy the textures
	objInst->m_pColorTexture = std::move(object3D.m_pColorTexture);
	objInst->m_pNormalTexture = std::move(object3D.m_pNormalTexture);


	// Add it to the list of objects to render
    // VAO are not copyable, so we move it
    m_objectsToRenderList.push_back(std::move(objInst));
}


/*
* Load all the shaders
* 
* @return void
*/
void OpenGl3DWidget::loadShaders()
{
    if (m_pShader.loadShader("../shaders/vertex.glsl", "../shaders/fragment.glsl"))
    {
        std::cout << "Shader loaded successfully" << std::endl;
    }
}
