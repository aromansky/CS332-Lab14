#include "../include/Application.h" // Путь к главному классу Application
#include <iostream>
#include <cstdlib> // Для EXIT_SUCCESS и EXIT_FAILURE

/**
 * @brief Главная функция программы.
 * * Инициализирует и запускает 3D-приложение.
 * * @return int Код завершения программы.
 */
int main() {
    // Включение русскоязычного вывода в консоль (зависит от настроек ОС)
    // std::locale::global(std::locale("ru_RU.UTF-8"));

    // --- 1. Инициализация и Запуск ---
    try {
        std::cout << "Starting 3D Renderer Application..." << std::endl;

        // Создаем экземпляр Application с нужными размерами окна и заголовком.
        // Здесь происходит инициализация SFML, OpenGL, GLEW, 
        // загрузка шейдеров, мешей и настройка сцены.
        Application app(1280, 720, "OpenGL 3.3 Shading Models (Phong, Toon, Custom)");

        // Запуск главного цикла приложения (render, update, event handling)
        app.run();

        std::cout << "Application finished successfully." << std::endl;
        return EXIT_SUCCESS;

    }
    catch (const std::runtime_error& e) {
        // Перехват критических ошибок (например, не найдены шейдеры/меши/текстуры)
        std::cerr << "\nCRITICAL RUNTIME ERROR: " << e.what() << std::endl;
        std::cerr << "Application terminated due to a critical error." << std::endl;
        return EXIT_FAILURE;

    }
    catch (const std::exception& e) {
        // Перехват других стандартных исключений
        std::cerr << "\nUNEXPECTED ERROR: " << e.what() << std::endl;
        std::cerr << "Application terminated." << std::endl;
        return EXIT_FAILURE;

    }
    catch (...) {
        // Перехват всех остальных исключений
        std::cerr << "\nUNKNOWN FATAL ERROR: An unknown error occurred." << std::endl;
        std::cerr << "Application terminated." << std::endl;
        return EXIT_FAILURE;
    }
}