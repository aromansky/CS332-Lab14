#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
// Включаем основные компоненты
#include "Object.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "Light/Light.h"
#include "Light/PointLight.h"
#include "Light/DirectionalLight.h"
#include "Light/SpotLight.h"

/**
 * @brief Управляет всеми объектами, источниками света и логикой сцены.
 */
class Scene {
public:
    /**
     * @brief Конструктор сцены.
     * @param shaderMgr Ссылка на ShaderManager для доступа к шейдерам.
     */
    Scene(ShaderManager& shaderMgr);

    // Главный метод настройки: создание объектов, загрузка мешей, установка света
    void setupScene();

    // Обновление логики сцены (например, анимация)
    void update(float deltaTime);

    /**
     * @brief Отрисовка всей сцены.
     * @param camera Камера, с точки зрения которой происходит отрисовка.
     */
    void render(const Camera& camera);

private:
    // --- Контейнеры Сцены ---
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // --- Зависимости ---
    ShaderManager& shaderManager;

    // --- Приватные вспомогательные методы ---

    // 1. Создание и настройка требуемых источников света
    void setupLights();

    // 2. Создание объектов (мин. 5), загрузка мешей и применение материалов
    void setupObjects();

    /**
     * @brief Отправляет все данные об источниках света в активный шейдер.
     * @param shader Активный шейдер.
     */
    void sendLightDataToShader(Shader& shader);
};