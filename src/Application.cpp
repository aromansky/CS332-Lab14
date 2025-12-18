#include "../include/Application.h"

#include <iostream>
#include <stdexcept>

// ----------------------------------------------------------------------
// Конструктор и Инициализация
// ----------------------------------------------------------------------

Application::Application(int width, int height, const std::string& title)
    : firstMouse(true), lastX(width / 2.0f), lastY(height / 2.0f)
{
    if (!initialize(width, height, title)) {
        throw std::runtime_error("Application initialization failed.");
    }
}

bool Application::initialize(int width, int height, const std::string& title) {
    // Настройка контекста OpenGL
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    // Создание окна
    window.create(sf::VideoMode(width, height), title, sf::Style::Default, settings);
    window.setMouseCursorVisible(false); // Скрываем курсор для управления камерой

    // Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Включение тестирования глубины (для корректной отрисовки 3D)
    glEnable(GL_DEPTH_TEST);

    // Установка области просмотра
    glViewport(0, 0, width, height);

    // --- Инициализация компонентов ---
    try {
        shaderManager = std::make_unique<ShaderManager>();
        // ВАЖНО: загрузить все шейдеры (Phong, Toon, Custom) перед созданием сцены
        shaderManager->loadAllShaders();

        // Позиция камеры (например, Vec3(0, 2, 5))
        camera = std::make_unique<Camera>(sf::Vector3f(0.0f, 2.0f, 5.0f));
        camera->aspectRatio = (float)width / (float)height;
        camera->MovementSpeed = CAMERA_SPEED;
        camera->MouseSensitivity = MOUSE_SENSITIVITY;

        // Создание сцены и наполнение объектами
        scene = std::make_unique<Scene>(*shaderManager);
        scene->setupScene(); // Метод, где вы создаете 5+ объектов и свет

    }
    catch (const std::exception& e) {
        std::cerr << "Application component initialization failed: " << e.what() << std::endl;
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------
// Главный цикл
// ----------------------------------------------------------------------

void Application::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        handleEvents();
        update(deltaTime);
        render();

        window.display();
    }
}

// ----------------------------------------------------------------------
// Обработка Ввода и Обновление
// ----------------------------------------------------------------------

void Application::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::Closed:
            window.close();
            break;

        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Escape)
                window.close();
            break;

        case sf::Event::MouseMoved: {
            float xpos = (float)event.mouseMove.x;
            float ypos = (float)event.mouseMove.y;

            if (firstMouse) {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // Инвертировано

            lastX = xpos;
            lastY = ypos;

            camera->processMouseMovement(xoffset, yoffset);

            // Возвращаем курсор в центр для непрерывного вращения
            sf::Vector2i center(window.getSize().x / 2, window.getSize().y / 2);
            sf::Mouse::setPosition(center, window);
            lastX = center.x;
            lastY = center.y;
            break;
        }

        case sf::Event::MouseWheelScrolled:
            camera->processMouseScroll((float)event.mouseWheelScroll.delta);
            break;

        default:
            break;
        }
    }
}

void Application::update(float deltaTime) {
    // Обработка клавиатуры для движения камеры
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        camera->processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        camera->processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        camera->processKeyboard(CameraMovement::LEFT, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        camera->processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        camera->processKeyboard(CameraMovement::UP, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        camera->processKeyboard(CameraMovement::DOWN, deltaTime);

    // Здесь можно обновлять положение объектов (например, анимация)
    scene->update(deltaTime);
}

// ----------------------------------------------------------------------
// Отрисовка
// ----------------------------------------------------------------------

void Application::render() {
    // 1. Очистка буферов
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Установка цвета фона (темно-серый)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 2. Отрисовка сцены
    scene->render(*camera);
}