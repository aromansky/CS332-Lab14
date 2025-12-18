#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

// Используем стандартные типы для ясности
using Vec2 = sf::Vector2f;
using Vec3 = sf::Vector3f;

/**
 * @brief Класс-обертка для шейдерной программы OpenGL.
 */
class Shader {
public:
    // ID шейдерной программы OpenGL
    unsigned int ID;

    /**
     * @brief Конструктор, который считывает и строит шейдер.
     * @param vertexPath Путь к файлу вершинного шейдера (.vert).
     * @param fragmentPath Путь к файлу фрагментного шейдера (.frag).
     */
    Shader(const char* vertexPath, const char* fragmentPath);

    // Деструктор
    ~Shader();

    // --- Методы использования ---

    // Активирует шейдерную программу
    void use() const;

    // --- Методы передачи Uniforms ---

    // Общие:
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    // Векторы:
    void setVec2(const std::string& name, const Vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const Vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

    // Матрицы (4x4):
    // Принимает массив float[16] (как предполагалось в Camera.cpp)
    void setMat4(const std::string& name, const float* matrixData) const;

private:
    // Проверка ошибок компиляции и линковки
    void checkCompileErrors(unsigned int shader, std::string type);
};