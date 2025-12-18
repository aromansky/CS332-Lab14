#pragma once

#include "Texture.h" // Предполагаем, что Texture.h находится в папке Utils
#include <SFML/System/Vector3.hpp>
#include <string>
#include <iostream>
#include <memory> // Для std::shared_ptr

using Vec3 = sf::Vector3f;

/**
 * @brief Перечисление для выбора модели освещения.
 */
enum class LightingModel {
    PHONG,          // Модель Фонга
    TOON_SHADING,   // Мультяшное освещение
    CUSTOM_MODEL    // Третья модель (например, Cook-Torrance, Oren-Nayar и т.д.)
};

/**
 * @brief Определяет, как объект должен выглядеть и как на него влияет свет.
 */
class Material {
public:
    // --- Параметры освещения Фонга/Ладоверта ---

    // Ambient: Цвет рассеянного света (часть, которая всегда видна)
    Vec3 ambient;

    // Diffuse: Цвет диффузного света (основной цвет, зависит от угла света)
    Vec3 diffuse;

    // Specular: Цвет бликов (зеркальное отражение)
    Vec3 specular;

    // Shininess (n): Степень блика (чем больше, тем меньше блик и он острее)
    float shininess;

    // --- Ресурсы ---

    // Указатель на текстуру объекта
    std::shared_ptr<Texture> texture;

    // --- Модель освещения ---
    LightingModel model;

    /**
     * @brief Конструктор материала.
     * @param ambient Цвет ambient.
     * @param diffuse Цвет diffuse.
     * @param specular Цвет specular.
     * @param shininess Коэффициент shininess.
     * @param texturePtr Указатель на объект Texture.
     * @param lightingModel Выбранная модель освещения.
     */
    Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float shininess,
        std::shared_ptr<Texture> texturePtr, LightingModel lightingModel);

    // --- Методы ---

    // Активирует модель освещения (для выбора шейдера)
    LightingModel getLightingModel() const { return model; }

    // Возвращает текстуру
    const Texture& getTexture() const { return *texture; }
};