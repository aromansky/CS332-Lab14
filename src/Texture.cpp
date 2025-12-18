#include "../include/Texture.h"
#include <algorithm> // Для std::swap

// ----------------------------------------------------------------------
// Конструкторы и Деструктор
// ----------------------------------------------------------------------

Texture::Texture(const std::string& filePath, bool flipVertically)
    : textureID(0)
{
    sf::Image image;
    if (!image.loadFromFile(filePath)) {
        throw std::runtime_error("ERROR::TEXTURE: Failed to load image from path: " + filePath);
    }

    // SFML загружает изображения "вверх ногами" по сравнению с OpenGL.
    // Переворачиваем, чтобы (0, 0) было в нижнем левом углу (стандарт OpenGL).
    if (flipVertically) {
        image.flipVertically();
    }

    loadFromImage(image);
}

// Конструктор перемещения
Texture::Texture(Texture&& other) noexcept
    : textureID(other.textureID)
{
    // Обнуляем исходный объект, чтобы деструктор не удалил ресурс
    other.textureID = 0;
}

// Оператор присваивания перемещением
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        // Очищаем собственные старые ресурсы
        cleanUp();

        // Перемещаем ID
        textureID = other.textureID;

        // Обнуляем исходный объект
        other.textureID = 0;
    }
    return *this;
}

Texture::~Texture() {
    cleanUp();
}

void Texture::cleanUp() {
    if (textureID != 0) {
        glDeleteTextures(1, &textureID);
    }
}

// ----------------------------------------------------------------------
// Методы OpenGL
// ----------------------------------------------------------------------

void Texture::loadFromImage(const sf::Image& image) {
    // 1. Создание текстурного объекта OpenGL
    glGenTextures(1, &textureID);

    // 2. Привязка текстуры
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 3. Загрузка данных изображения в текстуру

    // Определение формата: RGBA (4 компонента) или RGB (3 компонента)
    GLenum format = (image.getPixelsPtr()[3] == 0) ? GL_RGB : GL_RGBA;

    glTexImage2D(
        GL_TEXTURE_2D,              // Цель: 2D-текстура
        0,                          // Уровень детализации (mipmap level)
        format,                     // Внутренний формат (как OpenGL будет хранить)
        image.getSize().x,          // Ширина
        image.getSize().y,          // Высота
        0,                          // Граница (всегда 0)
        format,                     // Формат входных данных
        GL_UNSIGNED_BYTE,           // Тип данных (каждый цвет 8 бит)
        image.getPixelsPtr()        // Указатель на данные пикселей
    );

    // 4. Настройка параметров фильтрации и наложения

    // Фильтрация (уменьшение)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Фильтрация (увеличение)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Наложение (S-координата)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // Наложение (T-координата)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 5. Генерация mipmap-уровней (для улучшения качества при удалении)
    glGenerateMipmap(GL_TEXTURE_2D);

    // 6. Отвязка текстуры
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int textureUnit) const {
    if (textureID == 0) {
        return;
    }

    // 1. Активация нужного текстурного слота (0 - по умолчанию)
    // GL_TEXTURE0, GL_TEXTURE1, ...
    glActiveTexture(GL_TEXTURE0 + textureUnit);

    // 2. Привязка текстуры к активному слоту
    glBindTexture(GL_TEXTURE_2D, textureID);
}