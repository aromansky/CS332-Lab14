#include "../include/MeshParser.h"
#include <fstream>
#include <sstream>
#include <map>

// ----------------------------------------------------------------------
// Вспомогательные функции
// ----------------------------------------------------------------------

// Реализация оператора сравнения для FaceIndex
bool MeshParser::FaceIndex::operator<(const FaceIndex& other) const {
    if (vertexIndex != other.vertexIndex) return vertexIndex < other.vertexIndex;
    if (uvIndex != other.uvIndex) return uvIndex < other.uvIndex;
    return normalIndex < other.normalIndex;
}

// Простая функция для разделения строки
std::vector<std::string> MeshParser::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Вспомогательная функция для добавления вершины или использования кешированной
static unsigned int addOrGetVertex(
    const MeshParser::FaceIndex& faceIndex,
    const std::vector<Vec3>& tempVertices,
    const std::vector<Vec2>& tempTexCoords,
    const std::vector<Vec3>& tempNormals,
    std::vector<Vertex>& vertices,
    std::map<MeshParser::FaceIndex, unsigned int>& vertexCache)
{
    // Проверяем, существует ли уже такая комбинация в кеше
    if (vertexCache.count(faceIndex)) {
        return vertexCache[faceIndex];
    }

    // Создаем новую вершину
    Vertex newVertex;

    // Проверяем границы массивов
    if (faceIndex.vertexIndex >= tempVertices.size()) {
        throw std::runtime_error("Invalid V index: " + std::to_string(faceIndex.vertexIndex));
    }
    newVertex.position = tempVertices[faceIndex.vertexIndex];

    if (faceIndex.uvIndex >= tempTexCoords.size()) {
        throw std::runtime_error("Invalid VT index: " + std::to_string(faceIndex.uvIndex));
    }
    newVertex.texCoords = tempTexCoords[faceIndex.uvIndex];

    if (faceIndex.normalIndex >= tempNormals.size()) {
        throw std::runtime_error("Invalid VN index: " + std::to_string(faceIndex.normalIndex));
    }
    newVertex.normal = tempNormals[faceIndex.normalIndex];

    // Добавляем вершину в список
    vertices.push_back(newVertex);
    unsigned int newIndex = vertices.size() - 1;

    // Кешируем комбинацию
    vertexCache[faceIndex] = newIndex;

    return newIndex;
}

// Вспомогательная функция для парсинга индекса грани (v/vt/vn)
static MeshParser::FaceIndex parseFaceIndex(const std::string& faceStr) {
    std::vector<std::string> tokens = MeshParser::split(faceStr, '/');

    if (tokens.empty()) {
        throw std::runtime_error("Invalid face index format: empty");
    }

    MeshParser::FaceIndex result;

    // v (обязательно)
    result.vertexIndex = std::stoul(tokens[0]) - 1;

    // vt (если присутствует)
    result.uvIndex = (tokens.size() > 1 && !tokens[1].empty()) ? std::stoul(tokens[1]) - 1 : 0;

    // vn (если присутствует)
    result.normalIndex = (tokens.size() > 2 && !tokens[2].empty()) ? std::stoul(tokens[2]) - 1 : 0;

    return result;
}

// ----------------------------------------------------------------------
// Основной метод парсинга
// ----------------------------------------------------------------------

Mesh MeshParser::parseObj(const std::string& filePath) {
    // 1. Временные хранилища для сырых данных из файла
    std::vector<Vec3> tempVertices;
    std::vector<Vec2> tempTexCoords;
    std::vector<Vec3> tempNormals;

    // 2. Конечные контейнеры для OpenGL
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // 3. Карта для оптимизации (кеширование):
    // Ключ: уникальная комбинация v/vt/vn. Значение: индекс в массиве 'vertices'.
    std::map<FaceIndex, unsigned int> vertexCache;

    // --- Открытие файла ---
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR::MESHPARSER: Could not open file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            // v (position)
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            // vt (texture coordinates)
            Vec2 uv;
            ss >> uv.x >> uv.y;
            // OBJ часто инвертирует Y-координату текстуры
            tempTexCoords.push_back({ uv.x, 1.0f - uv.y });
        }
        else if (prefix == "vn") {
            // vn (normal)
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }
        else if (prefix == "f") {
            // f (face) - может быть треугольник (3 вершины) или полигон (4+ вершины)
            std::vector<std::string> faceIndicesStr;
            std::string faceIndexStr;

            // Читаем все индексы для этой грани
            while (ss >> faceIndexStr) {
                faceIndicesStr.push_back(faceIndexStr);
            }

            // Должна быть минимум 3 вершины для треугольника
            if (faceIndicesStr.size() < 3) {
                std::cerr << "WARNING::MESHPARSER: Face has less than 3 vertices, skipping." << std::endl;
                continue;
            }

            // Триангуляция полигона (fan triangulation)
            // Берем первую вершину и генерируем треугольники из оставшихся вершин
            std::vector<FaceIndex> faceIndices;
            for (const auto& indexStr : faceIndicesStr) {
                try {
                    faceIndices.push_back(parseFaceIndex(indexStr));
                }
                catch (const std::exception& e) {
                    std::cerr << "WARNING::MESHPARSER: " << e.what() << ", skipping face." << std::endl;
                    faceIndices.clear();
                    break;
                }
            }

            // Если успешно спарсили все индексы
            if (!faceIndices.empty()) {
                // Триангулируем веер из первой вершины
                unsigned int firstVertexIdx = addOrGetVertex(
                    faceIndices[0], tempVertices, tempTexCoords, tempNormals,
                    vertices, vertexCache);

                for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
                    unsigned int secondVertexIdx = addOrGetVertex(
                        faceIndices[i], tempVertices, tempTexCoords, tempNormals,
                        vertices, vertexCache);

                    unsigned int thirdVertexIdx = addOrGetVertex(
                        faceIndices[i + 1], tempVertices, tempTexCoords, tempNormals,
                        vertices, vertexCache);

                    // Добавляем треугольник
                    indices.push_back(firstVertexIdx);
                    indices.push_back(secondVertexIdx);
                    indices.push_back(thirdVertexIdx);
                }
            }
        }
    }

    file.close();

    // Если меш пустой
    if (vertices.empty() || indices.empty()) {
        throw std::runtime_error("ERROR::MESHPARSER: No valid vertices or faces found in file: " + filePath);
    }

    // Возвращаем готовый Mesh, который автоматически вызовет setupMesh() в конструкторе
    return Mesh(vertices, indices);
}

// ----------------------------------------------------------------------
// Метод для трансформации куба в платформу
// ----------------------------------------------------------------------

Mesh MeshParser::parseCubeAsPlatform(const std::string& filePath,
                                      float scaleX,
                                      float scaleY,
                                      float scaleZ) {
    // Сначала парсим куб как обычно
    Mesh cubeMesh = parseObj(filePath);

    // Получаем вершины из куба и применяем аффинные преобразования
    // Аффинное преобразование масштабирования: P' = S * P
    // где S = [scaleX   0      0   ]
    //         [  0   scaleY   0   ]
    //         [  0      0   scaleZ ]
    //         [  0      0      0   1]

    // Так как мы не имеем прямого доступа к вершинам через публичный интерфейс,
    // мы повторяем парсинг, но с преобразованиями вершин
    
    std::vector<Vec3> tempVertices;
    std::vector<Vec2> tempTexCoords;
    std::vector<Vec3> tempNormals;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::map<FaceIndex, unsigned int> vertexCache;

    // Открытие файла
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("ERROR::MESHPARSER: Could not open file: " + filePath);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            // v (position) - применяем масштабирование здесь
            Vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            // Аффинное преобразование: масштабирование
            vertex.x *= scaleX;
            vertex.y *= scaleY;
            vertex.z *= scaleZ;
            tempVertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            // vt (texture coordinates)
            Vec2 uv;
            ss >> uv.x >> uv.y;
            tempTexCoords.push_back({ uv.x, 1.0f - uv.y });
        }
        else if (prefix == "vn") {
            // vn (normal) - нормали также нужно трансформировать
            Vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            // Нормали трансформируются через обратно-транспонированную матрицу
            // Для простого масштабирования: N' = (1/S) * N (затем нормализуем)
            normal.x /= scaleX;
            normal.y /= scaleY;
            normal.z /= scaleZ;
            // Нормализуем
            float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            if (length > 0.0001f) {
                normal.x /= length;
                normal.y /= length;
                normal.z /= length;
            }
            tempNormals.push_back(normal);
        }
        else if (prefix == "f") {
            // f (face)
            std::vector<std::string> faceIndicesStr;
            std::string faceIndexStr;

            while (ss >> faceIndexStr) {
                faceIndicesStr.push_back(faceIndexStr);
            }

            if (faceIndicesStr.size() < 3) {
                std::cerr << "WARNING::MESHPARSER: Face has less than 3 vertices, skipping." << std::endl;
                continue;
            }

            std::vector<FaceIndex> faceIndices;
            for (const auto& indexStr : faceIndicesStr) {
                try {
                    faceIndices.push_back(parseFaceIndex(indexStr));
                }
                catch (const std::exception& e) {
                    std::cerr << "WARNING::MESHPARSER: " << e.what() << ", skipping face." << std::endl;
                    faceIndices.clear();
                    break;
                }
            }

            if (!faceIndices.empty()) {
                unsigned int firstVertexIdx = addOrGetVertex(
                    faceIndices[0], tempVertices, tempTexCoords, tempNormals,
                    vertices, vertexCache);

                for (size_t i = 1; i < faceIndices.size() - 1; ++i) {
                    unsigned int secondVertexIdx = addOrGetVertex(
                        faceIndices[i], tempVertices, tempTexCoords, tempNormals,
                        vertices, vertexCache);

                    unsigned int thirdVertexIdx = addOrGetVertex(
                        faceIndices[i + 1], tempVertices, tempTexCoords, tempNormals,
                        vertices, vertexCache);

                    indices.push_back(firstVertexIdx);
                    indices.push_back(secondVertexIdx);
                    indices.push_back(thirdVertexIdx);
                }
            }
        }
    }

    file.close();

    if (vertices.empty() || indices.empty()) {
        throw std::runtime_error("ERROR::MESHPARSER: No valid vertices or faces found in file: " + filePath);
    }

    return Mesh(vertices, indices);
}