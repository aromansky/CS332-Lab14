#pragma once

#include "Light.h"
#include <SFML/System/Vector3.hpp>

using Vec3 = sf::Vector3f;

/**
 * @brief Класс, представляющий Точечный свет (Point Light).
 * Излучает свет из одной точки одинаково во все стороны.
 */
class PointLight : public Light {
public:
    // --- Параметры Точечного Света ---

    // Позиция источника света в мировых координатах
    Vec3 position;

    // --- Коэффициенты затухания (Attenuation) ---
    // Формула затухания: 1.0 / (constant + linear * distance + quadratic * distance^2)

    // Constant: Коэффициент постоянного затухания (обычно 1.0)
    float constant;

    // Linear: Коэффициент линейного затухания (зависит от distance)
    float linear;

    // Quadratic: Коэффициент квадратичного затухания (зависит от distance^2, самое сильное падение)
    float quadratic;

    /**
     * @brief Конструктор точечного света.
     * @param position Позиция источника.
     * @param color Цвет света (RGB).
     * @param constant Коэффициент постоянного затухания.
     * @param linear Коэффициент линейного затухания.
     * @param quadratic Коэффициент квадратичного затухания.
     */
    PointLight(const Vec3& position, const Vec3& color,
        float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

    virtual ~PointLight() = default;
};