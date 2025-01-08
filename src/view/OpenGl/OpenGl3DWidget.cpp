#include "OpenGl3DWidget.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QSurfaceFormat>
#include <cmath>

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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Setup basic lighting (optional)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 0.0f, 0.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    // Move back a bit to see the cube
    glTranslatef(0.0f, 0.0f, -5.0f);
    // Rotate cube for a better view
    static float angle = 0.0f;
    glRotatef(angle, 1.0f, 1.0f, 0.0f);
    angle += 1.0f;  // update rotation

    // Draw a colored cube using immediate mode
    glBegin(GL_QUADS);
    // Front face (z = 1.0f)
    glColor3f(1.0f, 0.0f, 0.0f);  // Red
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    // Back face (z = -1.0f)
    glColor3f(0.0f, 1.0f, 0.0f);  // Green
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    // Top face (y = 1.0f)
    glColor3f(0.0f, 0.0f, 1.0f);  // Blue
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    // Bottom face (y = -1.0f)
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    // Right face (x = 1.0f)
    glColor3f(0.0f, 1.0f, 1.0f);  // Cyan
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    // Left face (x = -1.0f)
    glColor3f(1.0f, 0.0f, 1.0f);  // Magenta
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();

    // Trigger a repaint for continuous rotation (use timer for production code)
    update();
}
