#include "../include/ShaderManager.h"
#include <iostream>

ShaderManager::ShaderManager() {
    // Конструктор
}

void ShaderManager::loadAllShaders() {
    std::cout << "INFO::SHADER_MANAGER: Loading all shaders..." << std::endl;

    try {
        // 1. Модель Phong
        // Использует общий вершинный шейдер и phong.frag
        shaders[LightingModel::PHONG] = std::make_unique<Shader>(
            BASE_VERTEX_PATH.c_str(),
            "src/res/shaders/phong.frag"
        );
        std::cout << "  - Loaded PHONG shader." << std::endl;

        // 2. Toon Shading
        // Использует общий вершинный шейдер и toon.frag
        shaders[LightingModel::TOON_SHADING] = std::make_unique<Shader>(
            BASE_VERTEX_PATH.c_str(),
            "src/res/shaders/toon.frag"
        );
        std::cout << "  - Loaded TOON SHADING shader." << std::endl;

        // 3. Произвольная модель (CUSTOM_MODEL)
        // Например, Cook-Torrance или Oren-Nayar.
        // Использует общий вершинный шейдер и custom.frag
        shaders[LightingModel::CUSTOM_MODEL] = std::make_unique<Shader>(
            BASE_VERTEX_PATH.c_str(),
            "src/res/shaders/custom.frag"
        );
        std::cout << "  - Loaded CUSTOM MODEL shader." << std::endl;

    }
    catch (const std::exception& e) {
        // Если произошла ошибка при загрузке или компиляции (например, файл не найден),
        // исключение будет перехвачено здесь.
        std::cerr << "FATAL ERROR::SHADER_MANAGER: Shader loading failed: " << e.what() << std::endl;
        // Перебрасываем исключение, чтобы остановить инициализацию приложения
        throw;
    }

    std::cout << "INFO::SHADER_MANAGER: All shaders successfully loaded." << std::endl;
}

Shader& ShaderManager::getShader(LightingModel model) {
    auto it = shaders.find(model);

    if (it == shaders.end()) {
        throw std::runtime_error("ERROR::SHADER_MANAGER: Requested shader model not found in manager.");
    }

    // Возвращаем ссылку на объект Shader
    return *(it->second);
}