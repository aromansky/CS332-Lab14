#include "../include/Scene.h"
#include "../include/MeshParser.h" // Для загрузки моделей
#include <cmath>

// ----------------------------------------------------------------------
// Конструктор
// ----------------------------------------------------------------------

Scene::Scene(ShaderManager& shaderMgr)
    : shaderManager(shaderMgr), activeSpotLightIndex(-1)
{
    // setupScene() вызывается из Application::initialize()
}

// ----------------------------------------------------------------------
// 1. Настройка Источников Света
// ----------------------------------------------------------------------

void Scene::setupLights() {
    // --- 1. Направленный свет (Солнце) ---
    // Находится бесконечно далеко, лучи параллельны.
    auto directionalLight = std::make_shared<DirectionalLight>(
        sf::Vector3f(-0.5f, -1.0f, -0.3f), // Направление (вниз, немного назад, влево)
        sf::Vector3f(1.0f, 1.0f, 0.95f),   // Интенсивность
        0.5f                               // Ambient-компонента
    );
    lights.push_back(directionalLight);

    // --- 2. Точечный свет (Лампочка) ---
    // Светит из точки во все стороны.
    auto pointLight = std::make_shared<PointLight>(
        sf::Vector3f(-5.0f, 6.0f, 3.0f),  // Положение (высоко слева и сзади)
        sf::Vector3f(0.5f, 0.7f, 1.0f),   // Интенсивность (голубоватый)
        1.0f, 0.05f, 0.01f                // Коэффициенты затухания
    );
    lights.push_back(pointLight);

    // --- 3. Прожектор (Фонарик) ---
    // Свет образует конус, направлен на два куба.
    auto spotLight = std::make_shared<SpotLight>(
        sf::Vector3f(1.0f, 5.0f, -3.0f),  // Положение (выше кубов, спереди, между ними по X)
        sf::Vector3f(0.0f, -1.0f, 0.3f),  // Направление (прямо вниз и немного назад к кубам)
        sf::Vector3f(1.0f, 0.8f, 0.5f),   // Интенсивность (оранжевый)
        std::cos(glm::radians(15.0f)),    // Inner CutOff (15 градусов) - более узкий конус
        std::cos(glm::radians(25.0f))     // Outer CutOff (25 градусов) - расширение конуса
    );
    lights.push_back(spotLight);

    // Индекс прожектора = 2 (после направленного и точечного света)
    activeSpotLightIndex = 2;
}

void Scene::setupObjects() {
    // Используем простой текстурный заполнитель для примера
    auto whiteTexture = std::make_shared<Texture>("src/res/textures/white_diffuse.png", false);

    // --- Загрузка Мешей ---
    // Предполагаем, что у вас есть эти OBJ-файлы в res/models/
    std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(MeshParser::parseObj("src/res/models/cube.obj"));
    std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>(MeshParser::parseObj("src/res/models/sphere.obj"));
    std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(MeshParser::parseCubeAsPlatform("src/res/models/plane.obj"));

    // --- Создание Материалов ---

    // 1. Материал PHONG (Зеркальный)
    auto matPhong = std::make_shared<Material>(
        sf::Vector3f(0.3f, 0.3f, 0.3f),   // Ambient
        sf::Vector3f(0.8f, 0.6f, 0.3f),   // Diffuse
        sf::Vector3f(1.0f, 1.0f, 1.0f),   // Specular
        64.0f, whiteTexture, LightingModel::PHONG
    );

    // 2. Материал TOON SHADING
    auto matToon = std::make_shared<Material>(
        sf::Vector3f(0.2f, 0.2f, 0.2f),   // Ambient (усилен)
        sf::Vector3f(0.3f, 0.8f, 0.3f),   // Diffuse
        sf::Vector3f(0.0f, 0.0f, 0.0f),   // Specular
        0.0f, whiteTexture, LightingModel::TOON_SHADING
    );

    // 3. Материал CUSTOM MODEL (Матовый)
    auto matCustom = std::make_shared<Material>(
        sf::Vector3f(0.2f, 0.2f, 0.2f),   // Ambient (усилен)
        sf::Vector3f(0.9f, 0.1f, 0.1f),   // Diffuse
        sf::Vector3f(0.0f, 0.0f, 0.0f),   // Specular
        1.0f, whiteTexture, LightingModel::CUSTOM_MODEL
    );

    // --- Создание Объектов (Минимум 5) ---

    // 1. Пол (Plane, Phong)
    auto floor = std::make_shared<Object>(planeMesh, matPhong);
    floor->setScale(sf::Vector3f(10.0f, 1.0f, 10.0f));
    objects.push_back(floor);

    // 2. Сфера 1 (Phong)
    auto sphere1 = std::make_shared<Object>(sphereMesh, matPhong);
    sphere1->setPosition(sf::Vector3f(-2.0f, 1.0f, 0.0f));
    objects.push_back(sphere1);

    // 3. Куб 1 (Toon)
    auto cube1 = std::make_shared<Object>(cubeMesh, matToon);
    cube1->setPosition(sf::Vector3f(0.0f, 1.0f, 0.0f));
    objects.push_back(cube1);

    // 4. Куб 2 (Custom)
    auto cube2 = std::make_shared<Object>(cubeMesh, matCustom);
    cube2->setPosition(sf::Vector3f(2.0f, 1.0f, 0.0f));
    objects.push_back(cube2);

    // 5. Сфера 2 (Toon)
    auto sphere2 = std::make_shared<Object>(sphereMesh, matToon);
    sphere2->setPosition(sf::Vector3f(4.0f, 1.0f, -2.0f));
    sphere2->setScale(sf::Vector3f(0.5f, 0.5f, 0.5f));
    objects.push_back(sphere2);
}

void Scene::sendLightDataToShader(Shader& shader) {
    int pointLightCount = 0;
    int spotLightCount = 0;

    // Проходим по всем источникам света на сцене
    for (const auto& light : lights) {

        // Направленный свет
        if (auto dirLight = std::dynamic_pointer_cast<DirectionalLight>(light)) {
            shader.setVec3("dirLight.direction", dirLight->direction);
            shader.setVec3("dirLight.color", dirLight->color);
            shader.setFloat("dirLight.ambientIntensity", dirLight->ambientIntensity);

            // Точечный свет
        }
        else if (auto pLight = std::dynamic_pointer_cast<PointLight>(light)) {
            std::string base = "pointLights[" + std::to_string(pointLightCount++) + "].";
            shader.setVec3(base + "position", pLight->position);
            shader.setVec3(base + "color", pLight->color);
            shader.setFloat(base + "constant", pLight->constant);
            shader.setFloat(base + "linear", pLight->linear);
            shader.setFloat(base + "quadratic", pLight->quadratic);

            // Прожектор
        }
        else if (auto sLight = std::dynamic_pointer_cast<SpotLight>(light)) {
            std::string base = "spotLights[" + std::to_string(spotLightCount++) + "].";
            shader.setVec3(base + "position", sLight->position);
            shader.setVec3(base + "direction", sLight->direction);
            shader.setVec3(base + "color", sLight->color);
            shader.setFloat(base + "cutOff", sLight->cutOff);
            shader.setFloat(base + "outerCutOff", sLight->outerCutOff);
            // ДОБАВЛЕНЫ КОЭФФИЦИЕНТЫ ЗАТУХАНИЯ!
            shader.setFloat(base + "constant", sLight->constant);
            shader.setFloat(base + "linear", sLight->linear);
            shader.setFloat(base + "quadratic", sLight->quadratic);
        }
    }

    // Передаем общее количество источников света (важно для циклов в шейдере)
    shader.setInt("numPointLights", pointLightCount);
    shader.setInt("numSpotLights", spotLightCount);
}

// ----------------------------------------------------------------------
// Управление прожектором
// ----------------------------------------------------------------------

void Scene::syncSpotLightWithCamera(const Camera& camera) {
    if (activeSpotLightIndex < 0 || activeSpotLightIndex >= lights.size()) {
        return;
    }

    auto spotLight = std::dynamic_pointer_cast<SpotLight>(lights[activeSpotLightIndex]);
    if (!spotLight) {
        return;
    }

    // Синхронизируем позицию с позицией камеры
    spotLight->position = camera.Position;

    // Синхронизируем направление с направлением вида камеры
    spotLight->direction = camera.Front;

    std::cout << "INFO::SPOTLIGHT: Synced to camera position (" 
              << camera.Position.x << ", " << camera.Position.y << ", " << camera.Position.z << ")" 
              << std::endl;
}

void Scene::increaseSpotLightInnerCutOff(float delta) {
    if (activeSpotLightIndex < 0 || activeSpotLightIndex >= lights.size()) {
        return;
    }

    auto spotLight = std::dynamic_pointer_cast<SpotLight>(lights[activeSpotLightIndex]);
    if (!spotLight) {
        return;
    }

    // Получаем текущий угол в градусах
    float currentAngleDeg = glm::degrees(glm::acos(spotLight->cutOff));
    float newAngleDeg = currentAngleDeg + delta;

    // Ограничиваем угол от 1 до 89 градусов
    newAngleDeg = glm::clamp(newAngleDeg, 1.0f, 89.0f);

    // Вычисляем новый внешний угол (на 10 градусов больше внутреннего)
    float outerAngleDeg = newAngleDeg + 10.0f;
    outerAngleDeg = glm::clamp(outerAngleDeg, 1.0f, 89.0f);

    spotLight->cutOff = glm::cos(glm::radians(newAngleDeg));
    spotLight->outerCutOff = glm::cos(glm::radians(outerAngleDeg));

    std::cout << "INFO::SPOTLIGHT: Inner CutOff angle changed to " << newAngleDeg << " degrees" << std::endl;
}

void Scene::decreaseSpotLightInnerCutOff(float delta) {
    increaseSpotLightInnerCutOff(-delta);
}

float Scene::getSpotLightInnerCutOffDegrees() const {
    if (activeSpotLightIndex < 0 || activeSpotLightIndex >= lights.size()) {
        return -1.0f;
    }

    auto spotLight = std::dynamic_pointer_cast<SpotLight>(lights[activeSpotLightIndex]);
    if (!spotLight) {
        return -1.0f;
    }

    return glm::degrees(glm::acos(spotLight->cutOff));
}

// ----------------------------------------------------------------------
// Главный метод настройки
// ----------------------------------------------------------------------

void Scene::setupScene() {
    try {
        setupLights();
        setupObjects();
        std::cout << "INFO::SCENE: Scene initialized with " << objects.size() << " objects and " << lights.size() << " lights." << std::endl;
        std::cout << "INFO::SCENE: SpotLight index: " << activeSpotLightIndex << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "FATAL ERROR::SCENE: Setup failed: " << e.what() << std::endl;
        throw;
    }
}

// ----------------------------------------------------------------------
// Обновление
// ----------------------------------------------------------------------

void Scene::update(float deltaTime) {
    // Пример анимации: вращаем первый куб (индекс 2)
    if (objects.size() > 2) {
        Vec3 currentRot = objects[2]->rotation;
        currentRot.y += 45.0f * deltaTime; // Вращаем на 45 градусов в секунду
        objects[2]->setRotation(currentRot);
    }
}

// ----------------------------------------------------------------------
// Отрисовка
// ----------------------------------------------------------------------

void Scene::render(const Camera& camera) {
    float viewMatrix[16];
    float projMatrix[16];
    camera.getViewMatrix(viewMatrix);
    camera.getProjectionMatrix(projMatrix);

    // Проходим по каждому объекту и рисуем его
    for (const auto& object : objects) {
        // 1. Определяем, какой шейдер нужен объекту
        LightingModel requiredModel = object->getMaterial().getLightingModel();
        Shader& currentShader = shaderManager.getShader(requiredModel);
        currentShader.use();

        // 2. Передача глобальных матриц
        currentShader.setMat4("view", viewMatrix);
        currentShader.setMat4("projection", projMatrix);

        // 3. Передача позиции наблюдателя (камеры)
        currentShader.setVec3("viewPos", camera.Position);

        // 4. Передача данных источников света
        sendLightDataToShader(currentShader);

        // 5. Отрисовка объекта (передает Матрицу Модели и параметры Материала)
        object->draw(currentShader);
    }
}

// ----------------------------------------------------------------------
// Вспомогательный метод для света
// ------------------------------------------