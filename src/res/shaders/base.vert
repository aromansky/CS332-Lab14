#version 330 core

// --- Входные данные (Атрибуты Вершин) ---
layout (location = 0) in vec3 aPos;    // Позиция вершины (локальные координаты)
layout (location = 1) in vec3 aNormal; // Вектор нормали
layout (location = 2) in vec2 aTexCoords; // Текстурные координаты

// --- Выходные данные (Переменные, передаваемые во Фрагментный Шейдер) ---
out vec3 FragPos;      // Позиция фрагмента в мировом пространстве
out vec3 Normal;       // Нормаль в мировом пространстве
out vec2 TexCoords;    // Текстурные координаты

// --- Uniform-переменные (Матрицы и Камера) ---
uniform mat4 model;       // Матрица Модели (объект -> мир)
uniform mat4 view;        // Матрица Вида (мир -> камера)
uniform mat4 projection;  // Матрица Проекции (камера -> экран)

void main()
{
    // 1. Расчет финальной позиции вершины (Clip Space)
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // 2. Расчет позиции фрагмента в мировом пространстве
    // (Используем vec4(aPos, 1.0) для учета смещения (Translation) в матрице Модели)
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // 3. Расчет нормали в мировом пространстве
    // (Используем mat3(transpose(inverse(model))) для корректной трансформации нормалей
    // при неравномерном масштабировании. Здесь используем просто model matrix 3x3,
    // предполагая равномерное масштабирование или что model matrix не содержит non-uniform scale)
    Normal = mat3(model) * aNormal;
    
    // 4. Передача текстурных координат
    TexCoords = aTexCoords;
}