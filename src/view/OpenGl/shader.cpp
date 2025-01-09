// Includes from project
#include "shader.hpp"

// Includes from STL
#include <iostream>
#include <stdexcept>
#include <filesystem>


namespace fs = std::filesystem;


/*
* Class for loading shaders
* If the shaders are not loaded or linked correctly, an exception is thrown
*
* @param vertexPath Path to the vertex shader
* @param fragmentPath Path to the fragment shader
*/
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	if (!loadShader(vertexPath, fragmentPath))
	{
		throw std::runtime_error("Failed to load shader");
	}
}


bool Shader::loadShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    if (!fs::exists(vertexPath))
    {
        std::cerr << "Vertex shader file does not exist : " << vertexPath << std::endl;
        return false;
    }

    if (!fs::exists(fragmentPath))
    {
        std::cerr << "Fragment shader file does not exist : " << fragmentPath << std::endl;
        return false;
    }

    // Load shaders
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath.c_str()))
    {
        // Get the error message from the exception
        QString errorLog = m_shaderProgram.log();
        std::string errorMsg = errorLog.toStdString();
        std::cerr << "Failed to load vertex shader :" << std::endl <<  errorMsg << std::endl;
        return false;
    }
    if (!m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath.c_str()))
    {
        // Get the error message from the exception
        QString errorLog = m_shaderProgram.log();
        std::string errorMsg = errorLog.toStdString();
        std::cerr << "Failed to load fragment shader :" << std::endl << errorMsg << std::endl;
        return false;
    }

    // Link shaders
    if (!m_shaderProgram.link())
    {
        // Get the error message from the exception
        QString errorLog = m_shaderProgram.log();
        std::string errorMsg = errorLog.toStdString();
        std::cerr << "Failed to link shader program :" << std::endl << errorMsg << std::endl;
        return false;
    }
}