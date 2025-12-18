#pragma once

#include "Light.h" // Предполагаемый базовый класс
#include <SFML/System/Vector3.hpp>
#include <cmath> // Для std::cos

using Vec3 = sf::Vector3f;

/**
 * @brief Класс, представляющий источник света типа "Прожектор" (SpotLight).
 * Излучает свет из точки в заданном направлении внутри конуса.
 */
class SpotLight : public Light {
public:
    // --- Параметры Прожектора ---

    // Позиция источника света в мировых координатах
    Vec3 position;

    // Направление, куда светит прожектор (нормализованный вектор)
    Vec3 direction;

    // CutOff: Косинус угла внутреннего конуса. 
    // float, который используется в шейдере для проверки, находится ли пиксель внутри конуса.
    float cutOff;

    // OuterCutOff: Косинус угла внешнего конуса (для мягкого края).
    float outerCutOff;

    // --- Параметры затухания (Attenuation) ---
    // Часто используются для симуляции реального падения интенсивности света
    float constant;
    float linear;
    float quadratic;

    /**
     * @brief Конструктор прожектора.
     * @param position Позиция источника.
     * @param direction Направление света.
     * @param color Цвет света (RGB).
     * @param cutOffAngle Угол внутреннего конуса (в радианах или уже косинус).
     * @param outerCutOffAngle Угол внешнего конуса (в радианах или уже косинус).
     * @param constant Коэффициент постоянного затухания.
     * @param linear Коэффициент линейного затухания.
     * @param quadratic Коэффициент квадратичного затухания.
     */
    SpotLight(const Vec3& position, const Vec3& direction, const Vec3& color,
        float cutOffAngle, float outerCutOffAngle,
        float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

    // Метод base::Light::getType() может возвращать LightType::SPOT
    // или можно использовать dynamic_pointer_cast в Scene.cpp
};