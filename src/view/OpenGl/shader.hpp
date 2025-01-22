#pragma once

// Includes from 3rd party
#include <QOpenGLShaderProgram>

// Include from STL
#include <string>


/*
* Class for loading shaders
*/
class Shader
{
public:
    QOpenGLShaderProgram m_shaderProgram;

public:
    Shader(
        const std::string& vertexPath,
        const std::string& tessControlPath,
        const std::string& tessEvalPath,
        const std::string& fragmentPath
    );
	Shader() {};
    ~Shader() {};

	bool loadShader(
        const std::string& vertexPath, 
        const std::string& tessControlPath,
        const std::string& tessEvalPath,
        const std::string& fragmentPath
    );

    QOpenGLShaderProgram& getShaderProgram() { return m_shaderProgram; }
};