#pragma once

#include "Mesh.h"
#include <string>
#include <stdexcept>
#include <iostream>

/**
 * @brief Класс для парсинга файлов 3D моделей (например, OBJ).
 */
class MeshParser {
public:
    /**
     * @brief Парсит OBJ-файл и возвращает готовый объект Mesh.
     * * @param filePath Путь к файлу .obj.
     * @return Mesh Готовый объект Mesh, готовый к загрузке в OpenGL.
     * @throws std::runtime_error Если файл не найден или имеет некорректный формат.
     */
    static Mesh parseObj(const std::string& filePath);

    /**
     * @brief Трансформирует куб в платформу, используя аффинные преобразования.
     * @param filePath Путь к файлу куба .obj.
     * @param scaleX Масштабирование по оси X (ширина платформы).
     * @param scaleY Масштабирование по оси Y (высота платформы).
     * @param scaleZ Масштабирование по оси Z (глубина платформы).
     * @return Mesh Объект платформы.
     */
    static Mesh parseCubeAsPlatform(const std::string& filePath,
                                     float scaleX = 5.0f,
                                     float scaleY = 0.2f,
                                     float scaleZ = 3.0f);

    // Структура для временного хранения индекса грани (v/vt/vn)
    struct FaceIndex {
        unsigned int vertexIndex;
        unsigned int uvIndex;
        unsigned int normalIndex;

        // Оператор сравнения для использования в std::map
        bool operator<(const FaceIndex& other) const;
    };

    // Вспомогательная функция для разделения строки по разделителю
    static std::vector<std::string> split(const std::string& s, char delimiter);
};