// Includes from project
#include "OpenGl3DWidget.hpp"

// Includes from 3rd party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QTimer>

// Includes from STL
#include <cmath>
#include <iostream>

/*
* OpenGl3DWidget constructor
*/
OpenGl3DWidget::OpenGl3DWidget(std::function<void()> initDrawCallback, QWidget* pParent) : m_InitDrawCallback(initDrawCallback), QOpenGLWidget(pParent)
{
    // Request a compatibility profile for fixed-function pipeline support
    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setVersion(4, 6);  // Requesting an older version for compatibility
    setFormat(format);

	// Create a timer to trigger the paintGL() function
    QTimer* pTimer = new QTimer(this);
    connect(pTimer, &QTimer::timeout, this, QOverload<>::of(&OpenGl3DWidget::update));
    pTimer->start(16);
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

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

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
    m_lightPosition = QVector3D(-3.0f, 10.0f, -3.0f);
    //m_cameraPosition = QVector3D(1.0f, 3.0f, 1.0f);
    m_cameraPosition = QVector3D(-1.0f, 3.0f, -1.0f);
    //QVector3D cameraLookAt(1.5f, 2.75f, 1.5f);
    QVector3D cameraLookAt(1.5f, 2.0f, 1.5f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_view = QMatrix4x4();
    m_view.lookAt(
        m_cameraPosition,             // Camera position
        cameraLookAt,               // Point looked by the camera
        QVector3D(0.0f, 1.0f, 0.0f) // Up vector
    );

    m_projection = QMatrix4x4();
    m_projection.perspective(45.0f, float(width()) / height(), 0.1f, 100.0f);

    // Update all cloths meshs
    m_InitDrawCallback();
    
	// draw all 3d objects
	for (auto& renderer : m_objectsToRenderList)
	{
        // Lock the mutex to prevent the physics simulation thread to access the vertices data
        std::lock_guard<std::mutex> lock(renderer->m_mutex);

        // Resend the vertices' data to the GPU if the mesh is dynamic
		if (!renderer->m_isStatic)
		{
            updateObject3D(renderer);
		}

		// Draw the object
        drawObject(renderer);
	}
}


/*
* Draw a 3D object at his position and scale
* 
* @param pObjRender ObjectRenderingInstance to draw
* @return void
*/
void OpenGl3DWidget::drawObject(std::shared_ptr<ObjectRenderingInstance> pObjRender)
{
    QMatrix4x4 model;
    model.setToIdentity();
    // Apply the translation
    model.translate(pObjRender->m_pPosRotScale->m_position[0], pObjRender->m_pPosRotScale->m_position[1], pObjRender->m_pPosRotScale->m_position[2]);
    // Apply the scale
    model.scale(pObjRender->m_pPosRotScale->m_scale[0], pObjRender->m_pPosRotScale->m_scale[1], pObjRender->m_pPosRotScale->m_scale[2]);

    // Activate the shader and define the uniformes
    m_pShader.m_shaderProgram.bind();

    // Set the uniforms
    // matrix
    m_pShader.m_shaderProgram.setUniformValue("model", model);
    m_pShader.m_shaderProgram.setUniformValue("view", m_view);
    m_pShader.m_shaderProgram.setUniformValue("projection", m_projection);
    // Light pos and camera pos
    m_pShader.m_shaderProgram.setUniformValue("lightPos", m_lightPosition);
    m_pShader.m_shaderProgram.setUniformValue("viewPos", m_cameraPosition);
    // Bump
    m_pShader.m_shaderProgram.setUniformValue("displacementScale", 0.05f);
    m_pShader.m_shaderProgram.setUniformValue("tessellationFactor", 40.0f);

    // Set textures
    // Color (diffuse)
    bool colorTextureAvailable = (pObjRender->m_pColorTexture != nullptr);
    m_pShader.m_shaderProgram.setUniformValue("useColorTexture", colorTextureAvailable);

    if (colorTextureAvailable)
    {
        glActiveTexture(GL_TEXTURE0);  // Select texture 0 unit
        if (pObjRender->m_pColorTexture)
        {
            pObjRender->m_pColorTexture->bind();
        }
        m_pShader.m_shaderProgram.setUniformValue("colorTexture", 0);  // Link sampler to texture unit 0
    }

    // Normal
    bool normalTextureAvailable = (pObjRender->m_pNormalTexture != nullptr);
    m_pShader.m_shaderProgram.setUniformValue("useNormalTexture", normalTextureAvailable);
    if (normalTextureAvailable)
    {
        glActiveTexture(GL_TEXTURE1);  // Select texture 1 unit
        if (pObjRender->m_pNormalTexture)
        {
            pObjRender->m_pNormalTexture->bind();
        }
        m_pShader.m_shaderProgram.setUniformValue("normalTexture", 1);  // Link sampler to texture unit 1
    }

    // Bump
    bool bumpTextureAvailable = (pObjRender->m_pBumpTexture != nullptr);
    m_pShader.m_shaderProgram.setUniformValue("useBumpTexture", bumpTextureAvailable);
    if (bumpTextureAvailable)
    {
        glActiveTexture(GL_TEXTURE2);  // Select texture 2 unit
        if (pObjRender->m_pBumpTexture)
        {
            pObjRender->m_pBumpTexture->bind();
        }
        m_pShader.m_shaderProgram.setUniformValue("bumpTexture", 2);  // Link sampler to texture unit 2
    }

    pObjRender->m_vao.bind();
    GLenum drawMode = GL_TRIANGLES;
    if (m_pShader.m_useTessellation)
    {
        glPatchParameteri(GL_PATCH_VERTICES, 3);  // 3 vertices per patch (triangle)
        drawMode = GL_PATCHES;
    }
    glDrawArrays(drawMode, 0, GLsizei(pObjRender->m_verticesData.size())); // GL_PATCHES for tessellation
    pObjRender->m_vao.release();

    m_pShader.m_shaderProgram.release();
}



/*
* Initialize a 3D object renderer (VAo & VBO) from an Object3D
* and move it to the list of objects to render
* 
* @param object3D Object3D to initialize
* @return void
*/
std::shared_ptr<ObjectRenderingInstance> OpenGl3DWidget::initialyzeObject3D(Object3D& object3D)
{
    std::cout << "Initialize VAO & VBO" << std::endl;
    // Force the OpenGL context to be the current one
    makeCurrent();

	std::shared_ptr<ObjectRenderingInstance> objInst = std::make_shared<ObjectRenderingInstance>();
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

    // Share the adresses of the position, rotation and scale
	objInst->m_pPosRotScale = std::make_shared<ObjectHandle>();

	// Copy the textures
	objInst->m_pColorTexture = object3D.m_pColorTexture;
	objInst->m_pNormalTexture = object3D.m_pNormalTexture;
    objInst->m_pBumpTexture = object3D.m_pBumpTexture;

	// Add it to the list of objects to render
    m_objectsToRenderList.push_back(objInst);

    return objInst;
}


/*
* Update the 3D object renderer (push the new vertices data to the graphic card's memory)
* 
* @param objInst ObjectRenderingInstance to update
* @return void
*/
void OpenGl3DWidget::updateObject3D(std::shared_ptr<ObjectRenderingInstance> pObjInst)
{
    pObjInst->m_vao.bind();
    pObjInst->m_vbo.bind();

    // Same size, no need to reallocate
	//pObjInst->m_vbo.allocate(pObjInst->m_verticesData.data(), int(pObjInst->m_verticesData.size() * sizeof(VBOVertex)));

    pObjInst->m_vbo.write(0, pObjInst->m_verticesData.data(), static_cast<int>(pObjInst->m_verticesData.size() * sizeof(VBOVertex)));

    pObjInst->m_vbo.release();
    pObjInst->m_vao.release();
}


/*
* Load all the shaders
* 
* @return void
*/
void OpenGl3DWidget::loadShaders()
{
    if (m_pShader.loadShader("../shaders/vertex.glsl", "../shaders/tessellationControl.glsl", "../shaders/tessellationEvaluation.glsl", "../shaders/fragment.glsl"))
    //if (m_pShader.loadShader("../shaders/vertex.glsl", "", "", "../shaders/fragment.glsl"))
    {
        std::cout << "Shader loaded successfully" << std::endl;
    }
}


/*
* Add a 3D object to the list of objects to render
* 
* @param object3D Object3D to add
* @return std::shared_ptr<ObjectRenderingInstance> Pointer to the object handle (to access its vertices, position, rotation and scale)
*/
std::shared_ptr<ObjectRenderingInstance> OpenGl3DWidget::addObject(Object3D& object3D)
{
	return initialyzeObject3D(object3D);
}

/*
* Remove all the objects from the list of objects to render
* 
* @return void
*/
void OpenGl3DWidget::removeAllObjects()
{
	m_objectsToRenderList.clear();
}


/*
* Remove a 3D object from the list of objects to render
* 
* @param pObject ObjectRenderingInstance to remove
* @return void
*/
void OpenGl3DWidget::removeObject(std::shared_ptr<ObjectRenderingInstance> pObject)
{
	for (auto it = m_objectsToRenderList.begin(); it != m_objectsToRenderList.end(); ++it)
	{
		if (*it == pObject)
		{
			m_objectsToRenderList.erase(it);
			return;
		}
	}
}
