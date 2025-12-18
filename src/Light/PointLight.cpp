#include "../../include/Light/PointLight.h"

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------

PointLight::PointLight(const Vec3& position, const Vec3& color,
    float constant, float linear, float quadratic)
    // Вызов конструктора базового класса
    : Light(color),
    position(position),
    constant(constant),
    linear(linear),
    quadratic(quadratic)
{
    // Инициализация завершена.
}