#pragma once

// Include from project
#include "vec3.hpp"

// Includes from 3rd party
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

// Include from STL
#include <string>
#include <vector>
#include <array>


/*
* VBOVertex struct for OpenGl rendering
*/
struct VBOVertex
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 2> uv; // If available
};


/*
* Object3D class
* 
* This class is used to represent a 3D object
*/
class Object3D
{
public:
	std::array<float, 3> m_position;
	std::array<float, 3> m_rotation;
	std::array<float, 3> m_scale;

private:
	std::vector<std::array<float, 3>> m_vertices;
	std::vector<std::array<float, 3>> m_normals;
	std::vector<std::array<float, 2>> m_uvs;
	std::vector<std::array<int, 9>> m_faces;

public:
	Object3D() {};
	Object3D(const std::array<float, 3>& position, const std::array<float, 3>& rotation, const std::array<float, 3>& scale) : m_position(position), m_rotation(rotation), m_scale(scale) {}

	bool loadFromObjFile(const std::string& path);
	std::vector<VBOVertex> computeVBOVerticesData();

private:
	bool parseFaceLine(const std::string& line);
};