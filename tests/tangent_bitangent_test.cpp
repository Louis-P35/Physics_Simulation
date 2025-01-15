#include <gtest/gtest.h>
#include <cmath>
#include <array>
#include <tuple>

#include "../src/math/Vec3.hpp"
#include "../src/view/OpenGl/Object3D.hpp"

class Object3DTestWrapper : public Object3D
{
public:
    std::tuple<Vec3, Vec3> publicComputeTangentAndBitangentVector(
        const Vec3& p0, const Vec3& p1, const Vec3& p2,
        const std::array<float, 2>& uv0,
        const std::array<float, 2>& uv1,
        const std::array<float, 2>& uv2
    ) {
        return computeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);
    }
};


bool approximatelyEqual(double a, double b, double epsilon = 1e-5)
{
    return std::fabs(a - b) < epsilon;
}

void assertVec3Near(const Vec3& v1, const Vec3& v2, double epsilon = 1e-5)
{
    EXPECT_NEAR(v1.x, v2.x, epsilon);
    EXPECT_NEAR(v1.y, v2.y, epsilon);
    EXPECT_NEAR(v1.z, v2.z, epsilon);
}


TEST(TangentBitangentTest, SimpleTriangle)
{
    // Definition of triangle and UVs
    Vec3 p0(0.0f, 0.0f, 0.0f);
    Vec3 p1(1.0f, 0.0f, 0.0f);
    Vec3 p2(0.0f, 1.0f, 0.0f);

    std::array<float, 2> uv0 = { 0.0f, 0.0f };
    std::array<float, 2> uv1 = { 1.0f, 0.0f };
    std::array<float, 2> uv2 = { 0.0f, 1.0f };

    Object3DTestWrapper obj;

    // Test compute function
    auto [tangent, bitangent] = obj.publicComputeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);

    // Expected values
    Vec3 expectedTangent(1.0f, 0.0f, 0.0f);
    Vec3 expectedBitangent(0.0f, 1.0f, 0.0f);

    // Check
    assertVec3Near(tangent, expectedTangent);
    assertVec3Near(bitangent, expectedBitangent);
}

TEST(TangentBitangentTest, BackFaceTriangle)
{
    // Définition d'un triangle sur la face arrière du cube
    // Normale attendue : (0, 0, -1)
    Vec3 p0(-1.0f, -1.0f, -1.0f);
    Vec3 p1(1.0f, -1.0f, -1.0f);
    Vec3 p2(1.0f, 1.0f, -1.0f);

    std::array<float, 2> uv0 = { 0.0f, 0.0f };
    std::array<float, 2> uv1 = { 1.0f, 0.0f };
    std::array<float, 2> uv2 = { 1.0f, 1.0f };

    Object3DTestWrapper obj;
    auto [tangent, bitangent] = obj.publicComputeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);

    // Valeurs attendues pour un triangle sur la face arrière
    Vec3 expectedTangent(1.0f, 0.0f, 0.0f);  // Selon UV horizontaux croissants
    Vec3 expectedBitangent(0.0f, 1.0f, 0.0f); // Selon UV verticaux croissants

    assertVec3Near(tangent, expectedTangent);
    assertVec3Near(bitangent, expectedBitangent);
}

TEST(TangentBitangentTest, RightFaceTriangle)
{
    // Définition d'un triangle sur la face droite du cube
    // Normale attendue : (1, 0, 0)
    Vec3 p0(1.0f, -1.0f, -1.0f);
    Vec3 p1(1.0f, -1.0f, 1.0f);
    Vec3 p2(1.0f, 1.0f, 1.0f);

    std::array<float, 2> uv0 = { 0.0f, 0.0f };
    std::array<float, 2> uv1 = { 1.0f, 0.0f };
    std::array<float, 2> uv2 = { 1.0f, 1.0f };

    Object3DTestWrapper obj;
    auto [tangent, bitangent] = obj.publicComputeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);

    // Valeurs attendues pour un triangle sur la face droite
    Vec3 expectedTangent(0.0f, 0.0f, 1.0f);  // Direction z positive
    Vec3 expectedBitangent(0.0f, 1.0f, 0.0f); // Direction y positive

    assertVec3Near(tangent, expectedTangent);
    assertVec3Near(bitangent, expectedBitangent);
}

TEST(TangentBitangentTest, TopFaceTriangle)
{
    // Définition d'un triangle sur la face dessus du cube
    // Normale attendue : (0, 1, 0)
    Vec3 p0(-1.0f, 1.0f, -1.0f);
    Vec3 p1(1.0f, 1.0f, -1.0f);
    Vec3 p2(1.0f, 1.0f, 1.0f);

    std::array<float, 2> uv0 = { 0.0f, 0.0f };
    std::array<float, 2> uv1 = { 1.0f, 0.0f };
    std::array<float, 2> uv2 = { 1.0f, 1.0f };

    Object3DTestWrapper obj;
    auto [tangent, bitangent] = obj.publicComputeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);

    // Valeurs attendues pour un triangle sur la face dessus
    Vec3 expectedTangent(1.0f, 0.0f, 0.0f);   // Direction x positive
    Vec3 expectedBitangent(0.0f, 0.0f, 1.0f); // Direction z positive

    assertVec3Near(tangent, expectedTangent);
    assertVec3Near(bitangent, expectedBitangent);
}

TEST(TangentBitangentTest, TriangleWithNegativeUVs)
{
    // Définition d'un triangle dans le plan XY
    Vec3 p0(0.0f, 0.0f, 0.0f);
    Vec3 p1(1.0f, 0.0f, 0.0f);
    Vec3 p2(0.0f, 1.0f, 0.0f);

    // Coordonnées UV choisies pour induire des valeurs négatives
    std::array<float, 2> uv0 = { 0.0f, 0.0f };
    std::array<float, 2> uv1 = { -1.0f, 0.0f };  // Valeur négative pour u
    std::array<float, 2> uv2 = { 0.0f, -1.0f };  // Valeur négative pour v

    Object3DTestWrapper obj;
    auto [tangent, bitangent] = obj.publicComputeTangentAndBitangentVector(p0, p1, p2, uv0, uv1, uv2);

    // Calcul attendu pour ce triangle :
    // deltaPos1 = p1 - p0 = (1,0,0)
    // deltaPos2 = p2 - p0 = (0,1,0)
    // deltaUV1 = uv1 - uv0 = (-1,0)
    // deltaUV2 = uv2 - uv0 = (0,-1)
    // f = 1/( (-1 * -1) - (0*0) ) = 1/1 = 1
    // Expected tangent = (deltaPos1 * (-1) - deltaPos2 * 0) * 1 = (-1,0,0)
    // Expected bitangent = (deltaPos2 * (-1) - deltaPos1 * 0) * 1 = (0,-1,0)

    Vec3 expectedTangent(-1.0f, 0.0f, 0.0f);
    Vec3 expectedBitangent(0.0f, -1.0f, 0.0f);

    assertVec3Near(tangent, expectedTangent);
    assertVec3Near(bitangent, expectedBitangent);
}
