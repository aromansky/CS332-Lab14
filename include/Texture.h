#pragma once

#include <GL/glew.h>
#include <SFML/Graphics/Image.hpp>
#include <string>
#include <stdexcept>
#include <iostream>

/**
 * @brief Класс-обертка для управления текстурным объектом OpenGL.
 */
class Texture {
public:
    // --- Конструкторы ---

    // Загружает изображение с помощью SFML и создает текстуру OpenGL
    Texture(const std::string& filePath, bool flipVertically = true);

    // Запрещаем копирование (т.к. содержит ресурс OpenGL)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Разрешаем перемещение (полезно для использования в контейнерах)
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    // --- Деструктор ---
    ~Texture();

    // --- Методы ---

    // Привязывает текстуру к активному текстурному слоту (unit)
    void bind(unsigned int textureUnit = 0) const;

    // Возвращает ID текстурного объекта OpenGL
    unsigned int getID() const { return textureID; }

private:
    unsigned int textureID;

    // --- Приватные методы ---

    // Освобождение ресурса OpenGL
    void cleanUp();

    // Загрузка и создание текстуры из объекта sf::Image
    void loadFromImage(const sf::Image& image);
};