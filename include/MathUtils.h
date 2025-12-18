#pragma once

#include <SFML/System/Vector3.hpp>

// »спользуем псевдоним Vec3 (sf::Vector3f)
using Vec3 = sf::Vector3f;

namespace MathUtils {

    void createLookAt(const Vec3& position, const Vec3& target, const Vec3& up, float result[16]);
    void createPerspective(float fov, float aspect, float nearP, float farP, float result[16]);

    // --- ќбъ€влени€ дл€ Object.cpp (Model Matrix) ---
    void createIdentityMatrix(float matrix[16]);
    void multiplyMatrix4x4(const float A[16], const float B[16], float result[16]);
    void createTranslationMatrix(const Vec3& t, float result[16]);
    void createScaleMatrix(const Vec3& s, float result[16]);
    void createRotationMatrix(const Vec3& r, float result[16]);
}