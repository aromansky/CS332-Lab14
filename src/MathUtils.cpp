#include "../include/Material.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Для lookAt и perspective
#include <glm/gtc/type_ptr.hpp>          // Для преобразования glm::mat4 в float*
#include <cstring>                       // Для std::memcpy (хотя type_ptr обычно достаточно)

namespace MathUtils {

    // Вспомогательная функция для GLM
    glm::vec3 toGLM(const Vec3& v) {
        return glm::vec3(v.x, v.y, v.z);
    }

    // Вспомогательная функция для очистки
    void createIdentityMatrix(float matrix[16]) {
        glm::mat4 I(1.0f); // Матрица идентичности GLM
        std::memcpy(matrix, glm::value_ptr(I), 16 * sizeof(float));
    }

    // --- Model Matrix Components ---

    void createTranslationMatrix(const Vec3& t, float result[16]) {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), toGLM(t));
        std::memcpy(result, glm::value_ptr(T), 16 * sizeof(float));
    }

    void createScaleMatrix(const Vec3& s, float result[16]) {
        glm::mat4 S = glm::scale(glm::mat4(1.0f), toGLM(s));
        std::memcpy(result, glm::value_ptr(S), 16 * sizeof(float));
    }

    void createRotationMatrix(const Vec3& r, float result[16]) {
        glm::mat4 R(1.0f);
        // Вращение в порядке XYZ (или ZYX, в зависимости от предпочтений)
        // Вращение по оси X (Pitch)
        R = glm::rotate(R, glm::radians(r.x), glm::vec3(1.0f, 0.0f, 0.0f));
        // Вращение по оси Y (Yaw)
        R = glm::rotate(R, glm::radians(r.y), glm::vec3(0.0f, 1.0f, 0.0f));
        // Вращение по оси Z (Roll)
        R = glm::rotate(R, glm::radians(r.z), glm::vec3(0.0f, 0.0f, 1.0f));

        std::memcpy(result, glm::value_ptr(R), 16 * sizeof(float));
    }

    // --- Matrix Multiplication (T * R * S) ---

    void multiplyMatrix4x4(const float A[16], const float B[16], float result[16]) {
        // Загружаем входные массивы в GLM-матрицы
        glm::mat4 matA = glm::make_mat4(A);
        glm::mat4 matB = glm::make_mat4(B);

        // Выполняем умножение
        glm::mat4 matResult = matA * matB;

        // Копируем результат обратно
        std::memcpy(result, glm::value_ptr(matResult), 16 * sizeof(float));
    }

    // ----------------------------------------------------------------------
    // 1. Создание матрицы Вида (LookAt Matrix) с GLM
    // ----------------------------------------------------------------------

    void createLookAt(const Vec3& position, const Vec3& target, const Vec3& up, float result[16]) {
        // 1. Преобразование входных SFML векторов в GLM векторы
        glm::vec3 glmPos = toGLM(position);
        glm::vec3 glmTarget = toGLM(target);
        glm::vec3 glmUp = toGLM(up);

        // 2. Использование функции GLM lookAt
        glm::mat4 view = glm::lookAt(glmPos, glmTarget, glmUp);

        // 3. Копирование данных из glm::mat4 в массив float[16]
        // glm::value_ptr возвращает указатель на массив float[16] в столбцово-мажорном порядке.
        std::memcpy(result, glm::value_ptr(view), 16 * sizeof(float));
    }

    // ----------------------------------------------------------------------
    // 2. Создание матрицы Проекции (Perspective Projection Matrix) с GLM
    // ----------------------------------------------------------------------

    void createPerspective(float fov, float aspect, float nearP, float farP, float result[16]) {
        // 1. Угол обзора (fov) в GLM perspective должен быть в радианах

        // 2. Использование функции GLM perspective
        glm::mat4 projection = glm::perspective(
            glm::radians(fov), // Преобразование градусов в радианы
            aspect,
            nearP,
            farP
        );

        // 3. Копирование данных из glm::mat4 в массив float[16]
        std::memcpy(result, glm::value_ptr(projection), 16 * sizeof(float));
    }
}