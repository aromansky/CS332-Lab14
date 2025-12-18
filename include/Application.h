#pragma once

#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <memory>
#include <string>

#include "Camera.h"
#include "Scene.h" 
#include "ShaderManager.h" 

class Application {
public:
    // --- Константы ---
    static constexpr int DEFAULT_WIDTH = 1280;
    static constexpr int DEFAULT_HEIGHT = 720;
    static constexpr float MOUSE_SENSITIVITY = 0.1f;
    static constexpr float CAMERA_SPEED = 5.0f;

    Application(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, const std::string& title = "OpenGL/SFML Renderer");

    // Главный цикл приложения
    void run();

private:
    // --- SFML/OpenGL Ресурсы ---
    sf::RenderWindow window;

    // --- Компоненты Сцены ---
    std::unique_ptr<Scene> scene;
    std::unique_ptr<Camera> camera;
    std::unique_ptr<ShaderManager> shaderManager;

    // --- Управление Вводом ---
    bool firstMouse;
    float lastX;
    float lastY;

    // --- Приватные методы ---

    // Инициализация SFML и OpenGL/GLEW
    bool initialize(int width, int height, const std::string& title);

    // Обработка всех событий (ввод, закрытие окна)
    void handleEvents();

    // Обновление логики сцены
    void update(float deltaTime);

    // Отрисовка сцены
    void render();
};