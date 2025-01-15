#pragma once

// Include from project
#include "../src/math/vec3.hpp"

// Includes from 3rd party
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

// Include from STL
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <tuple>


/*
* VBOVertex struct for OpenGl rendering
*/
struct VBOVertex
{
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 2> uv; // If available

	// Tangent and bitangent vectors for normal mapping
	std::array<float, 3> tangent;
	std::array<float, 3> bitangent;
};


struct ObjectHandle
{
	std::array<float, 3> m_position = { 0.0f, 0.0f, 0.0f };
	std::array<float, 3> m_rotation = { 0.0f, 0.0f, 0.0f };
	std::array<float, 3> m_scale = { 1.0f, 1.0f, 1.0f };
};


/*
* Object3D class
* 
* This class is used to represent a 3D object
*/
class Object3D
{
public:
	std::shared_ptr<QOpenGLTexture> m_pColorTexture;
	std::shared_ptr<QOpenGLTexture> m_pNormalTexture;

private:
	std::vector<std::array<float, 3>> m_vertices;
	std::vector<std::array<float, 3>> m_normals;
	std::vector<std::array<float, 2>> m_uvs;
	std::vector<std::array<int, 9>> m_faces;

	std::vector<Vec3> m_tangent;
	std::vector<Vec3> m_bitangent;


public:
	Object3D() {};

	bool loadFromObjFile(const std::string& path, const std::string& filename);
	std::vector<VBOVertex> computeVBOVerticesData();

protected:
	bool parseFaceLine(const std::string& line);
	std::shared_ptr<QOpenGLTexture> loadTexture(const std::string& path) const;
	bool computeTangentAndBitangentvectors();
	std::tuple<Vec3, Vec3> computeTangentAndBitangentVector(
		const Vec3& p0,
		const Vec3& p1,
		const Vec3& p2,
		const std::array<float, 2>& uv0,
		const std::array<float, 2>& uv1,
		const std::array<float, 2>& uv2
	);
};