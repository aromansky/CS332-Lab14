#include "../include/Camera.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm> // Для std::min/max и std::clamp
#include "../include/MathUtils.h"
// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------
Camera::Camera(Vec3 position, Vec3 up, float yaw, float pitch)
    : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch),
    MovementSpeed(SPEED_DEFAULT), MouseSensitivity(SENSITIVITY_DEFAULT), Zoom(ZOOM_DEFAULT),
    aspectRatio(800.0f / 600.0f), nearPlane(0.1f), farPlane(100.0f)
{
    // Инициализация фронтального вектора и других векторов
    updateCameraVectors();
}

// ----------------------------------------------------------------------
// Основные методы
// ----------------------------------------------------------------------

void Camera::getViewMatrix(float viewMatrix[16]) const {
    // Вызов внешней математической функции LookAt
    MathUtils::createLookAt(Position, Position + Front, Up, viewMatrix);
}

void Camera::getProjectionMatrix(float projectionMatrix[16]) const {
    // Вызов внешней математической функции Perspective
    MathUtils::createPerspective(Zoom, aspectRatio, nearPlane, farPlane, projectionMatrix);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    // Примечание: Для реалистичного "полета" камеры, 
    // движение UP/DOWN можно привязать к WorldUp, а не к локальному Up.

    if (direction == CameraMovement::FORWARD)
        Position += Front * velocity;
    if (direction == CameraMovement::BACKWARD)
        Position -= Front * velocity;
    if (direction == CameraMovement::LEFT)
        Position -= Right * velocity;
    if (direction == CameraMovement::RIGHT)
        Position += Right * velocity;
    if (direction == CameraMovement::UP) // Движение вверх/вниз относительно мира
        Position += WorldUp * velocity;
    if (direction == CameraMovement::DOWN)
        Position -= WorldUp * velocity;

    // Опционально: Для камеры, закрепленной на плоскости (например, от первого лица),
    // можно сбросить Y-координату: Position.y = 0.0f;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Ограничение угла Pitch, чтобы избежать "сальто" камеры
    if (constrainPitch) {
        Pitch = std::clamp(Pitch, -89.0f, 89.0f);
    }

    // Пересчет векторов после изменения углов
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
    // Изменение FOV (Zoom)
    Zoom -= yoffset;
    // Ограничение FOV
    Zoom = std::clamp(Zoom, 1.0f, 45.0f);
}

// ----------------------------------------------------------------------
// Приватная вспомогательная функция
// ----------------------------------------------------------------------

void Camera::updateCameraVectors() {
    // Вычисление нового вектора Front
    Vec3 newFront;
    // Преобразование углов из градусов в радианы
    const float yawRad = Yaw * (M_PI / 180.0f);
    const float pitchRad = Pitch * (M_PI / 180.0f);

    newFront.x = std::cos(yawRad) * std::cos(pitchRad);
    newFront.y = std::sin(pitchRad);
    newFront.z = std::sin(yawRad) * std::cos(pitchRad);

    // Нормализация Front (для простоты я предполагаю, что sf::Vector3f имеет normalize())
    // В C++ без GLM это будет ручная нормализация:
    float length = std::sqrt(newFront.x * newFront.x + newFront.y * newFront.y + newFront.z * newFront.z);
    if (length != 0.0f) {
        newFront.x /= length;
        newFront.y /= length;
        newFront.z /= length;
    }
    Front = newFront;

    // Пересчет вектора Right (Векторное произведение: Front x WorldUp)
    // Right = Front.cross(WorldUp).normalize(); // Если есть метод cross()

    // Ручное векторное произведение:
    Right.x = Front.y * WorldUp.z - Front.z * WorldUp.y;
    Right.y = Front.z * WorldUp.x - Front.x * WorldUp.z;
    Right.z = Front.x * WorldUp.y - Front.y * WorldUp.x;
    // Нормализация Right (аналогично Front)
    length = std::sqrt(Right.x * Right.x + Right.y * Right.y + Right.z * Right.z);
    if (length != 0.0f) {
        Right.x /= length;
        Right.y /= length;
        Right.z /= length;
    }

    // Пересчет вектора Up (Векторное произведение: Right x Front)
    // Up = Right.cross(Front).normalize(); // Если есть метод cross()

    // Ручное векторное произведение:
    Up.x = Right.y * Front.z - Right.z * Front.y;
    Up.y = Right.z * Front.x - Right.x * Front.z;
    Up.z = Right.x * Front.y - Right.y * Front.x;
    // Нормализация Up (аналогично Front)
    length = std::sqrt(Up.x * Up.x + Up.y * Up.y + Up.z * Up.z);
    if (length != 0.0f) {
        Up.x /= length;
        Up.y /= length;
        Up.z /= length;
    }
}