#include "../include/Object.h"
#include "../include/Shader.h"
#include "../include/MathUtils.h"
#include <cmath>
#include <cstring> // Для memcpy

// ----------------------------------------------------------------------
// Конструктор и Трансформации
// ----------------------------------------------------------------------

Object::Object(std::shared_ptr<Mesh> meshPtr, std::shared_ptr<Material> materialPtr)
    : mesh(meshPtr), material(materialPtr),
    position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f)
{
    // Инициализация матрицы модели как матрицы идентичности
    createIdentityMatrix(modelMatrix);
    updateModelMatrix();
}

void Object::createIdentityMatrix(float matrix[16]) {
    // Установка всех элементов в 0, а диагональных в 1
    std::memset(matrix, 0, 16 * sizeof(float));
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
}

void Object::updateModelMatrix() {
    float T[16], R[16], S[16];
    float tempMatrix[16];

    // 1. Создание матриц (вызываем MathUtils):
    MathUtils::createTranslationMatrix(position, T);
    MathUtils::createRotationMatrix(rotation, R);
    MathUtils::createScaleMatrix(scale, S);

    // 2. Объединение: R * S
    MathUtils::multiplyMatrix4x4(R, S, tempMatrix);

    // 3. Объединение: T * (R * S) -> T * tempMatrix
    MathUtils::multiplyMatrix4x4(T, tempMatrix, modelMatrix);
}

// ----------------------------------------------------------------------
// Методы доступа
// ----------------------------------------------------------------------

void Object::getModelMatrix(float modelMatrixArray[16]) const {
    std::memcpy(modelMatrixArray, modelMatrix, 16 * sizeof(float));
}

void Object::setPosition(const Vec3& newPos) {
    position = newPos;
    updateModelMatrix();
}

void Object::setRotation(const Vec3& newRot) {
    rotation = newRot;
    updateModelMatrix();
}

void Object::setScale(const Vec3& newScale) {
    scale = newScale;
    updateModelMatrix();
}

// ----------------------------------------------------------------------
// Метод отрисовки
// ----------------------------------------------------------------------

void Object::draw(const Shader& shader) const {
    if (!mesh || !material) {
        return;
    }

    // 1. Привязка текстуры (например, к слоту 0)
    material->getTexture().bind(0);

    // 2. Передача Uniforms, специфичных для этого объекта/материала

    // Матрица Модели (обязательна)
    shader.setMat4("model", modelMatrix);

    // Параметры материала (для Phong, Custom)
    shader.setVec3("material.ambient", material->ambient);
    shader.setVec3("material.diffuse", material->diffuse);
    shader.setVec3("material.specular", material->specular);
    shader.setFloat("material.shininess", material->shininess);

    // Установка слота текстуры в шейдере
    shader.setInt("material.texture_diffuse1", 0);

    // 3. Отрисовка геометрии
    mesh->draw();
}