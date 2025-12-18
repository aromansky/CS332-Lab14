#include "../../include/Light/SpotLight.h"

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------

SpotLight::SpotLight(const Vec3& position, const Vec3& direction, const Vec3& color,
    float cutOffAngle, float outerCutOffAngle,
    float constant, float linear, float quadratic)
    // Предполагаем, что базовый класс Light имеет конструктор для цвета
    : Light(color),
    position(position),
    direction(direction),
    cutOff(cutOffAngle), // Мы ожидаем, что сюда передается уже КОСИНУС угла
    outerCutOff(outerCutOffAngle),
    constant(constant),
    linear(linear),
    quadratic(quadratic)
{
    // В реальном проекте здесь может быть код для нормализации вектора direction,
    // если это не было сделано при вызове:
    // float length = std::sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
    // if (length > 0.0f) { direction /= length; }
}

// ----------------------------------------------------------------------
// Определение констант затухания (для справки)
// ----------------------------------------------------------------------
/* * Коэффициенты затухания определяют, как быстро свет гаснет с расстоянием.
 * Эти значения обычно подбираются экспериментально.
 * * Примерные значения для distance:
 * Distance | Constant | Linear | Quadratic
 * ---------|----------|--------|-----------
 * 7        | 1.0      | 0.7    | 1.8
 * 20       | 1.0      | 0.22   | 0.20
 * 50       | 1.0      | 0.09   | 0.032  <-- Используется в Scene.cpp
 * 100      | 1.0      | 0.045  | 0.0075
 */