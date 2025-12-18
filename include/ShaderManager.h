#pragma once

#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include "Shader.h"
#include "Material.h" // Для использования enum class LightingModel

/**
 * @brief Класс для загрузки, хранения и управления всеми шейдерными программами.
 */
class ShaderManager {
public:
    ShaderManager();

    // Запрещаем копирование
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;

    // Загружает все необходимые шейдеры из папки res/shaders/
    void loadAllShaders();

    /**
     * @brief Получает шейдерную программу по типу модели освещения.
     * @param model Тип модели освещения (PHONG, TOON_SHADING, CUSTOM_MODEL).
     * @return Shader& Ссылка на запрошенный шейдер.
     * @throws std::runtime_error Если шейдер не найден.
     */
    Shader& getShader(LightingModel model);

private:
    // Хранилище шейдерных программ. Ключ - тип модели, Значение - указатель на Shader.
    std::map<LightingModel, std::unique_ptr<Shader>> shaders;

    // Путь к общему вершинному шейдеру
    const std::string BASE_VERTEX_PATH = "src/res/shaders/base.vert";
};