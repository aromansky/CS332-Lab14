#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector3.hpp>

// Определения для удобства
using Vec3 = sf::Vector3f;

// Перечисление для направления движения камеры
enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    // Константы для настроек камеры по умолчанию
    static constexpr float YAW_DEFAULT = -90.0f;
    static constexpr float PITCH_DEFAULT = 0.0f;
    static constexpr float SPEED_DEFAULT = 5.0f;
    static constexpr float SENSITIVITY_DEFAULT = 0.1f;
    static constexpr float ZOOM_DEFAULT = 45.0f;

    // --- Параметры камеры ---

    // Векторы
    Vec3 Position;
    Vec3 Front;
    Vec3 Up;
    Vec3 Right;
    Vec3 WorldUp; // Глобальный вектор "вверх" (обычно Vec3(0.0f, 1.0f, 0.0f))

    // Углы Эйлера (градусы)
    float Yaw;
    float Pitch;

    // Настройки
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom; // Угол поля зрения (FOV)

    // Размеры окна для матрицы проекции
    float aspectRatio;
    float nearPlane;
    float farPlane;

    // --- Конструкторы ---

    Camera(Vec3 position = Vec3(0.0f, 0.0f, 3.0f),
        Vec3 up = Vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW_DEFAULT,
        float pitch = PITCH_DEFAULT);

    // --- Основные методы ---

    // Получить матрицу вида (View Matrix)
    // Матрица 4x4, хранится в массиве float[16] (Column-major order)
    void getViewMatrix(float viewMatrix[16]) const;

    // Получить матрицу проекции (Projection Matrix)
    void getProjectionMatrix(float projectionMatrix[16]) const;

    // Обработка ввода с клавиатуры
    void processKeyboard(CameraMovement direction, float deltaTime);

    // Обработка ввода с мыши (движения)
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // Обработка ввода с мыши (скролла для зума/FOV)
    void processMouseScroll(float yoffset);

private:
    // Вспомогательная функция для пересчета векторов Front, Right, Up
    void updateCameraVectors();
};