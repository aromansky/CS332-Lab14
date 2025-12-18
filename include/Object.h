#pragma once

#include "Mesh.h"
#include "Material.h"
#include <memory>
#include <SFML/System/Vector3.hpp>

// Используем стандартные типы для ясности
using Vec3 = sf::Vector3f;

/**
 * @brief Представляет отдельный 3D-объект на сцене.
 * Связывает геометрию (Mesh), внешний вид (Material) и трансформации.
 */
class Object {
public:
    // --- Положение и Трансформации ---

    // Положение в мировых координатах
    Vec3 position;

    // Вращение (Эйлеровы углы или кватернион, здесь используем Vec3 для углов в градусах)
    Vec3 rotation;

    // Масштаб
    Vec3 scale;

    /**
     * @brief Конструктор объекта.
     * @param meshPtr Указатель на Mesh (геометрию).
     * @param materialPtr Указатель на Material (внешний вид и освещение).
     */
    Object(std::shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> materialPtr);

    // --- Методы ---

    // Обновляет внутреннюю матрицу модели (Model Matrix)
    void updateModelMatrix();

    // Получает матрицу модели (4x4)
    // Предполагается, что она хранится в массиве float[16]
    void getModelMatrix(float modelMatrix[16]) const;

    // Отрисовывает объект
    void draw(const class Shader& shader) const;

    // Получает материал для выбора шейдера
    const Material& getMaterial() const { return *material; }

    // --- Методы трансформации ---
    void setPosition(const Vec3& newPos);
    void setRotation(const Vec3& newRot);
    void setScale(const Vec3& newScale);

private:
    // --- Ресурсы ---
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

    // --- Внутренние данные ---
    // Матрица трансформации (Model Matrix: Translation * Rotation * Scale)
    float modelMatrix[16];

    // Вспомогательная функция для создания матрицы идентичности
    void createIdentityMatrix(float matrix[16]);
};