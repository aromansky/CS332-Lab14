#include "../include/Material.h"

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------

Material::Material(const Vec3& ambient, const Vec3& diffuse, const Vec3& specular, float shininess,
    std::shared_ptr<Texture> texturePtr, LightingModel lightingModel)
    : ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    shininess(shininess),
    texture(texturePtr),
    model(lightingModel)
{
    // Проверка на корректность указателя текстуры
    if (!texture) {
        std::cerr << "WARNING::MATERIAL: Texture pointer is null." << std::endl;
        // Можно загрузить пустую/белую текстуру-заполнитель для безопасной работы
    }
}

// ----------------------------------------------------------------------
// Применение материала в процессе отрисовки (Псевдокод)
// ----------------------------------------------------------------------

/* * В реальной реализации этот метод не будет находиться в Material.cpp.
 * Он будет находиться в Scene.cpp или Object.cpp, но здесь показано,
 * как данные Material используются для передачи в шейдер.

void applyMaterial(const Shader& shader, const Material& material) {
    // Активация текстуры
    material.getTexture().bind(0);

    // Передача коэффициентов освещения в шейдер
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);
    shader.setFloat("material.shininess", material.shininess);

    // Сообщение шейдеру о том, что диффузная текстура находится в слоте 0
    shader.setInt("material.texture_diffuse1", 0);
}
*/