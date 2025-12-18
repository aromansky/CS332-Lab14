#include "../../include/Light/DirectionalLight.h"
#include <cmath>
#include <iostream>

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------

DirectionalLight::DirectionalLight(const Vec3& direction, const Vec3& color, float ambientIntensity)
// Вызов конструктора базового класса
    : Light(color),
    direction(direction),
    ambientIntensity(ambientIntensity)
{
    // В реальном проекте здесь должна быть нормализация вектора direction, 
    // чтобы убедиться, что его длина равна 1.0. 

    // Пример ручной нормализации:
    float length = std::sqrt(this->direction.x * this->direction.x +
        this->direction.y * this->direction.y +
        this->direction.z * this->direction.z);

    if (length > 0.0f) {
        this->direction.x /= length;
        this->direction.y /= length;
        this->direction.z /= length;
    }
    else {
        std::cerr << "WARNING::DIRECTIONAL_LIGHT: Direction vector is zero." << std::endl;
        // Установка направления по умолчанию, если вектор нулевой
        this->direction = Vec3(0.0f, -1.0f, 0.0f);
    }
}