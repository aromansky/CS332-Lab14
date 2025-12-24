#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
// Подключаем необходимые компоненты
#include "Object.h"
#include "Camera.h"
#include "ShaderManager.h"
#include "Light/Light.h"
#include "Light/PointLight.h"
#include "Light/DirectionalLight.h"
#include "Light/SpotLight.h"

/**
 * @brief Основной класс сцены, содержащий объекты, источники света и камеру.
 */
class Scene {
public:
    /**
     * @brief Конструктор класса.
     * @param shaderMgr Ссылка на ShaderManager для работы с шейдерами.
     */
    Scene(ShaderManager& shaderMgr);

    // Главный метод инициализации: создание объектов, добавление источников света
    void setupScene();

    // Обновление сцены на каждый кадр (анимация, движение)
    void update(float deltaTime);

    /**
     * @brief Рендеринг всей сцены.
     * @param camera Камера, в которой видны объекты и источники света.
     */
    void render(const Camera& camera);

    // --- Управление прожектором (SpotLight) ---

    /**
     * @brief Синхронизирует прожектор с позицией и направлением камеры.
     * @param camera Текущая камера.
     */
    void syncSpotLightWithCamera(const Camera& camera);

    /**
     * @brief Увеличивает внутренний угол конуса прожектора.
     * @param delta Изменение угла в градусах.
     */
    void increaseSpotLightInnerCutOff(float delta);

    /**
     * @brief Уменьшает внутренний угол конуса прожектора.
     * @param delta Уменьшение угла в градусах.
     */
    void decreaseSpotLightInnerCutOff(float delta);

    /**
     * @brief Возвращает текущий внутренний угол конуса прожектора (в градусах).
     * @return Угол в градусах.
     */
    float getSpotLightInnerCutOffDegrees() const;

    // --- Управление точечным источником света (PointLight) ---

    /**
     * @brief Синхронизирует точечный источник света с позицией камеры.
     * @param camera Текущая камера.
     */
    void syncPointLightWithCamera(const Camera& camera);

private:
    // --- Содержимое сцены ---
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;

    // --- Индекс активного прожектора ---
    int activeSpotLightIndex = -1;

    // --- Индекс активного точечного источника света ---
    int activePointLightIndex = -1;

    // --- Конструктор ---
    ShaderManager& shaderManager;

    // --- Методы вспомогательных операций ---

    // 1. Инициализация и управление источниками света и объектами
    void setupLights();

    // 2. Создание объектов (мин. 5), добавление текстур и установка параметров
    void setupObjects();

    /**
     * @brief Отправляет данные всех источников света в активный шейдер.
     * @param shader Активный шейдер.
     */
    void sendLightDataToShader(Shader& shader);
};