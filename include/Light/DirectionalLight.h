#pragma once

#include "Light.h"
#include <SFML/System/Vector3.hpp>

using Vec3 = sf::Vector3f;

/**
 * @brief Класс, представляющий Направленный свет (Directional Light).
 * Симулирует свет от бесконечно удаленного источника (параллельные лучи).
 */
class DirectionalLight : public Light {
public:
    // --- Параметры Направленного Света ---

    // Вектор, указывающий направление света (например, от Солнца к сцене)
    Vec3 direction;

    // Ambient-компонента (фоновый свет)
    float ambientIntensity;

    /**
     * @brief Конструктор направленного света.
     * @param direction Направление света (нормализованный вектор).
     * @param color Цвет света (RGB).
     * @param ambientIntensity Интенсивность ambient-компоненты (от 0.0 до 1.0).
     */
    DirectionalLight(const Vec3& direction, const Vec3& color, float ambientIntensity = 0.1f);

    virtual ~DirectionalLight() = default;
};