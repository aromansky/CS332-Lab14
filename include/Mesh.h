#pragma once

#include <vector>
#include <GL/glew.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

// Используем стандартные типы для ясности
using Vec2 = sf::Vector2f;
using Vec3 = sf::Vector3f;

// --- 1. Структура Vertex (Вершина) ---
// Определяет, какие данные содержит один "экземпляр" вершины
struct Vertex {
    Vec3 position;  // Позиция (x, y, z)
    Vec3 normal;    // Нормаль (для освещения)
    Vec2 texCoords; // Текстурные координаты (u, v)
};

// --- 2. Класс Mesh ---

class Mesh {
public:
    // --- Данные меша ---
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // --- Конструкторы ---

    // Используется классом MeshParser для создания меша
    Mesh(const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices);

    // Запрещаем копирование (т.к. содержит ресурсы OpenGL)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Разрешаем перемещение
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // --- Деструктор ---
    ~Mesh();

    // --- Методы OpenGL ---

    // Загружает данные из векторов в буферы VAO/VBO/EBO
    void setupMesh();

    // Отрисовывает меш
    void draw() const;

private:
    // --- OpenGL Буферы ---
    unsigned int VAO, VBO, EBO; // Vertex Array Object, Vertex Buffer Object, Element Buffer Object

    // --- Приватные методы ---

    // Очистка буферов
    void cleanUp();
};