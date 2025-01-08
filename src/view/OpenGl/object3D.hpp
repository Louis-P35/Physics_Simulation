#pragma once

// Include from project
#include "vec3.hpp"

// Include from STL
#include <string>
#include <vector>
#include <array>


class Object3D
{
public:
	Vec3 m_position;
	Vec3 m_rotation;
	Vec3 m_scale;

private:
	std::vector<Vec3> m_vertices;
	std::vector<Vec3> m_normals;
	std::vector<std::array<int, 2>> m_uvs;

	std::vector<std::array<int, 9>> m_faces;

public:
	Object3D(const Vec3& position, const Vec3& rotation, const Vec3& scale) : m_position(position), m_rotation(rotation), m_scale(scale) {}

	bool loadFromObjFile(const std::string& path);

private:
	bool parseFaceLine(const std::string& line);
};