#include "../include/Mesh.h"
#include <iostream>
#include <stdexcept>

// ----------------------------------------------------------------------
// Конструкторы и Деструктор
// ----------------------------------------------------------------------

Mesh::Mesh(const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices), VAO(0), VBO(0), EBO(0)
{
    // Как только данные получены, сразу же настраиваем буферы OpenGL
    setupMesh();
}

// Конструктор перемещения
Mesh::Mesh(Mesh&& other) noexcept
    : vertices(std::move(other.vertices)),
    indices(std::move(other.indices)),
    VAO(other.VAO), VBO(other.VBO), EBO(other.EBO)
{
    // Очищаем перемещенные ресурсы
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

// Оператор присваивания перемещением
Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        // Очищаем собственные старые ресурсы
        cleanUp();

        // Перемещаем данные
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;

        // Очищаем перемещенные ресурсы
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

Mesh::~Mesh() {
    cleanUp();
}

void Mesh::cleanUp() {
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        VAO = VBO = EBO = 0;
    }
}

// ----------------------------------------------------------------------
// Методы OpenGL
// ----------------------------------------------------------------------

void Mesh::setupMesh() {
    // 1. Создание буферов
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 2. Привязка VAO (Vertex Array Object)
    glBindVertexArray(VAO);

    // 3. Привязка VBO (Vertex Buffer Object)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Загрузка данных вершин
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 4. Привязка EBO (Element Buffer Object)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Загрузка данных индексов
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // --- Указание атрибутов вершин ---
    // Это определяет, как OpenGL должен интерпретировать данные в буфере VBO.

    // A. Атрибут 0: Position (Позиция)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // B. Атрибут 1: Normal (Нормаль)
    // Размер 3 float-ов, смещение равно размеру Vec3 (Position)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // C. Атрибут 2: TexCoords (Текстурные координаты)
    // Размер 2 float-ов, смещение равно размеру Position + Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // 5. Отвязка VAO
    glBindVertexArray(0);
}

void Mesh::draw() const {
    if (VAO == 0 || indices.empty()) {
        // Ошибка или пустой меш, нечего рисовать.
        return;
    }

    // Привязка VAO, который содержит все настройки буферов
    glBindVertexArray(VAO);

    // Вызов отрисовки с использованием буфера элементов (EBO)
    // GL_TRIANGLES - рисуем треугольники
    // indices.size() - количество индексов
    // GL_UNSIGNED_INT - тип индексов
    // 0 - смещение
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Отвязка VAO
    glBindVertexArray(0);
}