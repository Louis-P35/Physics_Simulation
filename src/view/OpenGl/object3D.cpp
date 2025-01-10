// Include from project
#include "object3D.hpp"

// Includes from 3rd party
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QImage>

// Include from STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <cstddef>


namespace fs = std::filesystem;


/*
* Load object from file
* 
* @param path Path to the directory containing the file
* @param filename Name of the file
* @return bool True if the object is loaded successfully, false otherwise
*/
bool Object3D::loadFromObjFile(const std::string& path, const std::string& filename)
{
	const std::string fullPath = path + filename;
    bool hasNormal = false;
	bool hasUV = false;

	std::cout << "Loading object from file: " << path << std::endl;
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

	// Clean up the object
	m_vertices.clear();
	m_normals.clear();
	m_uvs.clear();

    // Open file in read only mode
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open the file " << fullPath << std::endl;
        return false;
    }
    std::cout << "File: " << fullPath << " opened successfully." << std::endl;

	// Read file line by line
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;

		iss >> prefix;

		// Check if the line is a vertex
        if (prefix == "v")
        {
            std::array<float, 3> vertex{0, 0, 0};
            iss >> vertex[0] >> vertex[1] >> vertex[2];
			m_vertices.push_back(vertex);
        }

        // Check if the line is a normal
        if (prefix == "vn")
        {
            hasNormal = true;
            std::array<float, 3> normal{0, 0, 0};
            iss >> normal[0] >> normal[1] >> normal[2];
            m_normals.push_back(normal);
        }

        // Check if the line is a texture coord
        if (prefix == "vt")
        {
			hasUV = true;
            std::array<float, 2> uv{0, 0};
            iss >> uv[0] >> uv[1];
            m_uvs.push_back(uv);
        }

        // Check if the line is a face
        if (prefix == "f")
        {
            if (!parseFaceLine(line))
            {
                // Closing the file
                file.close();

				return false;
            }
        }
    }

    // Closing the file
    file.close();

    std::cout << "File: " << fullPath << " loaded successfully." << std::endl;

	// Load the textures
    // Color
    const fs::path colorTexturePath = fs::path(path) / "textures" / "color.png";
    m_pColorTexture = loadTexture(colorTexturePath.string());
	if (m_pColorTexture != nullptr)
	{
		std::cout << "Color texture loaded successfully." << std::endl;
	}
    else
	{
		std::cerr << "Error: Failed to load color texture : " << colorTexturePath.string() << std::endl;
	}

    // Normal
    const fs::path normalTexturePath = fs::path(path) / "textures" / "normal.png";
    m_pNormalTexture = loadTexture(normalTexturePath.string());
    if (m_pNormalTexture != nullptr)
    {
        std::cout << "Normal texture loaded successfully." << std::endl;
    }
    else
    {
        std::cerr << "Error: Failed to load normal texture : " << normalTexturePath.string() << std::endl;
    }

	// Compute tangent and bitangent vectors (needed for normal mapping)
	if (hasNormal && hasUV)
	{
		computeTangentAndBitangentvectors();
	}

	return true;
}


/*
* Parse face line
* 
* @param line Line to parse
* @return bool True if the line is correctly parsed, false otherwise
*/
bool Object3D::parseFaceLine(const std::string& line)
{
    std::istringstream iss(line);
    std::string prefix;
	std::string vertexS;
    std::vector<std::array<int, 3>> face;

	// Check if the line is a face
	iss >> prefix;
	if (prefix != "f")
	{
		return false;
	}

	// Parse the face line
    while (iss >> vertexS)
    {
        std::istringstream edgeIss(vertexS);
        std::string segment;
		std::vector<std::string> edgeVect;

        while (std::getline(edgeIss, segment, '/'))
        {
			edgeVect.push_back(segment);
        }

		// Invalid face line check
		if (edgeVect.size() != 3)
		{
			std::cerr << "Error: Invalid face line: " << line << std::endl;
			return false;
		}

        int vertexIndex = -1;
		int uvIndex = -1;
		int normalIndex = -1;

        // Convert the string to int
        try
        {
            vertexIndex = std::stoi(edgeVect[0]) - 1;
        }
		catch (const std::invalid_argument& e)
		{
			std::cerr << "Error: Invalid vertex index: " << edgeVect[0] << std::endl;
			return false;
		}
        try
        {
            uvIndex = std::stoi(edgeVect[1]) - 1;
        }
        catch (const std::invalid_argument& e)
        {
        }

        try
        {
            normalIndex = std::stoi(edgeVect[2]) - 1;
        }
        catch (const std::invalid_argument& e)
        {
        }

		std::array<int, 3> faceData = {vertexIndex, uvIndex, normalIndex};
		face.push_back(faceData);
    }

	// Invalid face check
	if (face.size() != 3)
	{
		std::cerr << "Error: Invalid face (more than 3 vertices) line: " << line << std::endl;
		return false;
	}

	// Add the face to the list
    std::array<int, 9> faceData = {
        face[0][0], face[0][1], face[0][2],
        face[1][0], face[1][1], face[1][2],
        face[2][0], face[2][1], face[2][2]
        };
	m_faces.push_back(faceData);

	return true;
}


/*
* Compute VBO vertices data
* 
* @return std::vector<VBOVertex> List of VBO vertices data
*/
std::vector<VBOVertex> Object3D::computeVBOVerticesData()
{
    std::vector<VBOVertex> verticesData;

	for (const auto& face : m_faces)
	{
		for (int i = 0; i < 3; i++)
		{
			VBOVertex vertex;

			vertex.position = m_vertices[face[i * 3]];
            if (face[i * 3 + 1] != -1)
            {
                vertex.uv = m_uvs[face[i * 3 + 1]];
            }
            else
			{
				vertex.uv = { 0, 0 };
			}
            vertex.normal = m_normals[face[i * 3 + 2]];

			if (!m_tangent.empty() && !m_bitangent.empty())
			{
				vertex.tangent = m_tangent[face[i * 3]].toArray();
				vertex.bitangent = m_bitangent[face[i * 3]].toArray();
			}

            verticesData.push_back(vertex);
		}
	}

    return verticesData;
}


/*
* Load texture from file
* 
* @param path Path to the file
* @return std::unique_ptr<QOpenGLTexture> Pointer to the texture
*/
std::unique_ptr<QOpenGLTexture> Object3D::loadTexture(const std::string& path) const
{
	// Load the image from the disk
    QImage image(path.c_str());
    if (image.isNull())
    {
        std::cerr << "Failed to load texture image" << std::endl;
        return nullptr;
    }

	// Convert the image to the format that OpenGL expects
    QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);

	// Crate and configure the texture
    // Need to mirror the image to get it in line with the UVs...
    std::unique_ptr<QOpenGLTexture> pTexture(new QOpenGLTexture(glImage.mirrored(false, true)));

	// Configure the parameters of the texture
    pTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    pTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    pTexture->setWrapMode(QOpenGLTexture::Repeat);

	return std::move(pTexture); // std::move() not necessary ?
}


/*
* Compute tangent and bitangent vectors for all the faces (needed for normal mapping)
* 
* @return bool True if the computation is successful, false otherwise
*/
bool Object3D::computeTangentAndBitangentvectors()
{
	// Init tangent and bitangent vectors
	m_tangent.clear();
	m_tangent.resize(m_vertices.size(), {0.0f, 0.0f, 0.0f});
    m_bitangent.clear();
    m_bitangent.resize(m_vertices.size(), {0.0f, 0.0f, 0.0f});

	for (auto face : m_faces)
	{
		Vec3 p0 = Vec3(m_vertices[face[0]]);
		Vec3 p1 = Vec3(m_vertices[face[3]]);
		Vec3 p2 = Vec3(m_vertices[face[6]]);
        std::array<float, 2> uv0 = m_uvs[face[1]];
        std::array<float, 2> uv1 = m_uvs[face[4]];
        std::array<float, 2> uv2 = m_uvs[face[7]];
		Vec3 tangent, bitangent;

		std::tie(tangent, bitangent) = computeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);
        
		// Sum the vectors per vertex
        m_tangent[face[0]] += tangent;
        m_tangent[face[3]] += tangent;
        m_tangent[face[6]] += tangent;
        m_bitangent[face[0]] += bitangent;
        m_bitangent[face[3]] += bitangent;
        m_bitangent[face[6]] += bitangent;
	}

	// Normalize the vectors
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		m_tangent[i] = m_tangent[i].normalize();
		m_bitangent[i] = m_bitangent[i].normalize();
	}

	return true;
}


/*
* Compute tangent and bitangent vectors for a face (needed for normal mapping)
* 
* @param p0 First vertex of the face
* @param p1 Second vertex of the face
* @param p2 Third vertex of the face
* @param uv0 UV of the first vertex
* @param uv1 UV of the second vertex
* @param uv2 UV of the third vertex
* @return std::tuple<Vec3, Vec3> Tuple containing the tangent and bitangent vectors
*/
std::tuple<Vec3, Vec3> Object3D::computeTangentAndBitangentVector(
    const Vec3& p0, 
    const Vec3& p1, 
    const Vec3& p2, 
    const std::array<float, 2>& uv0,
    const std::array<float, 2>& uv1,
    const std::array<float, 2>& uv2
)
{
    // Compute direction vectors of the triangle on p0
    Vec3 deltaPos1 = p1 - p0;
    Vec3 deltaPos2 = p2 - p0;

    // UV coordinates delta
    float deltaUV1u = uv1[0] - uv0[0];
    float deltaUV1v = uv1[1] - uv0[1];
    float deltaUV2u = uv2[0] - uv0[0];
    float deltaUV2v = uv2[1] - uv0[1];

    // Compute the factor to solve the system
    float denom = (deltaUV1u * deltaUV2v - deltaUV2u * deltaUV1v);
    float f = (denom != 0.0f) ? (1.0f / denom) : 0.0f;

    // Compute tangent and bitangent vectors
    Vec3 tangent = (deltaPos1 * deltaUV2v - deltaPos2 * deltaUV1v) * f;
    Vec3 bitangent = (deltaPos2 * deltaUV1u - deltaPos1 * deltaUV2u) * f;

    // Normalize
    tangent = tangent.normalize();
    bitangent = bitangent.normalize();

    return std::make_tuple(tangent, bitangent);
}