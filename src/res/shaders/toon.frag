#version 330 core

// --- Входные данные из Вершинного Шейдера ---
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// --- Выходные данные ---
out vec4 FragColor;

// --- Uniform-переменные ---
uniform vec3 viewPos; 

// --- Структуры Света (Должны совпадать с phong.frag) ---
struct DirLight {
    vec3 direction;        
    vec3 color;            
    float ambientIntensity; 
};
uniform DirLight dirLight;

struct PointLight {
    vec3 position;  
    vec3 color;     
    float constant;
    float linear;
    float quadratic;
};
#define MAX_POINT_LIGHTS 4
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights; 

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};
#define MAX_SPOT_LIGHTS 2
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numSpotLights; 

// --- Структура Материала ---
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess; // Используется только для Specular-компоненты
    sampler2D texture_diffuse1; 
};
uniform Material material;

// --- Уровни дискретизации ---
// Используем для простоты 3 уровня: Тень, Полутень, Свет.
const float lightLevels[] = float[] (0.1, 0.4, 0.9);

/**
 * @brief Дискретизирует значение освещенности, чтобы получить ступенчатый эффект.
 */
float quantize(float intensity) {
    float result = lightLevels[0]; // Минимум (Тень)
    for (int i = 0; i < lightLevels.length(); i++) {
        if (intensity > lightLevels[i]) {
            result = lightLevels[i];
        }
    }
    return result;
}

// --- Функции для отдельных типов света ---

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(-light.direction);
    
    // 1. Ambient
    vec3 ambient = light.color * material.ambient * light.ambientIntensity;
    
    // 2. Diffuse (Цел-шейдинг)
    float diff = max(dot(norm, lightDir), 0.0);
    // Дискретизация диффузного света
    float quantizedDiff = quantize(diff);
    vec3 diffuse = light.color * material.diffuse;
    // Применяем дискретизированное значение к цвету текстуры
    vec3 finalDiffuse = diffuse * quantizedDiff;
    
    // 3. Specular (Простой бинарный блик)
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = dot(viewD, reflectDir);
    // Блик включается, только если он очень острый
    float specFactor = step(0.95, spec); // 1.0, если spec > 0.95, иначе 0.0
    vec3 specular = light.color * material.specular * specFactor;
    
    return ambient + finalDiffuse + specular;
}

// Функции для PointLight и SpotLight будут аналогичны DirLight, 
// но с добавлением Затухания и Конуса соответственно.
vec3 calculatePointLight(PointLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    // Затухание
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Диффузное освещение с дискретизацией
    float diff = max(dot(norm, lightDir), 0.0);
    float quantizedDiff = quantize(diff);
    vec3 diffuse = light.color * material.diffuse * quantizedDiff;
    
    // Бинарный блик
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = dot(viewD, reflectDir);
    float specFactor = step(0.95, spec);
    vec3 specular = light.color * material.specular * specFactor;
    
    // Ambient (не дискретизируем)
    vec3 ambient = light.color * material.ambient;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(SpotLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    // Затухание
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Конус
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    if (theta <= light.outerCutOff) intensity = 0.0;
    
    // Диффузное освещение с дискретизацией
    float diff = max(dot(norm, lightDir), 0.0);
    float quantizedDiff = quantize(diff);
    vec3 diffuse = light.color * material.diffuse * quantizedDiff;
    
    // Бинарный блик
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = dot(viewD, reflectDir);
    float specFactor = step(0.95, spec);
    vec3 specular = light.color * material.specular * specFactor;
    
    // Ambient
    vec3 ambient = light.color * material.ambient;

    return (ambient + diffuse + specular) * attenuation * intensity;
}

// --- Главная функция ---
void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewD = normalize(viewPos - FragPos);
    
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);
    
    vec3 result = vec3(0.0);
    
    // 1. Направленный свет
    result += calculateDirLight(dirLight, norm, viewD);
    
    // 2. Точечные источники света
    for(int i = 0; i < numPointLights; i++)
        result += calculatePointLight(pointLights[i], norm, viewD);
        
    // 3. Прожекторы
    for(int i = 0; i < numSpotLights; i++)
        result += calculateSpotLight(spotLights[i], norm, viewD);
        
    // Применяем цвет текстуры (модуляция)
    FragColor = vec4(result, 1.0) * texColor;
}