#version 330 core

// --- Входные данные из Вершинного Шейдера ---
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// --- Выходные данные ---
out vec4 FragColor;

// --- Uniform-переменные ---
uniform vec3 viewPos; // Позиция камеры (наблюдателя) в мировом пространстве

// --- Структуры Источников Света ---

// Направленный свет (Directional Light) - для Солнца
struct DirLight {
    vec3 direction;        
    vec3 color;            
    float ambientIntensity; 
};
uniform DirLight dirLight;
// Количество направленных источников света (обычно 1)
// В вашей Scene.cpp используется одна структура DirLight, а не массив, но добавим для будущего:
// uniform int numDirLights; 

// Точечный свет (Point Light) - для лампочек
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

// Прожектор (Spot Light) - для фонарика
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
    float shininess;
    sampler2D texture_diffuse1; // Слот для текстуры
};
uniform Material material;

// --- Основная функция расчета освещения Фонга ---
vec3 calculatePhong(vec3 lightDir, vec3 lightColor, vec3 ambientColor, vec3 fragNormal)
{
    // 1. Ambient (фоновый)
    vec3 ambient = ambientColor * material.ambient;
    
    // 2. Diffuse (рассеянный)
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = lightColor * material.diffuse * diff;
    
    // 3. Specular (зеркальный)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = normalize(reflect(-lightDir, fragNormal));  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = lightColor * material.specular * spec; 
    
    return ambient + diffuse + specular;
}

// --- Функции для отдельных типов света ---

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(-light.direction); // Направленный свет светит в обратную сторону вектора
    
    // Ambient
    vec3 ambient = light.color * material.ambient * light.ambientIntensity;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * material.diffuse * diff;
    
    // Specular
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewD, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color * material.specular * spec;
    
    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    // --- Затухание ---
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Ambient
    vec3 ambient = light.color * material.ambient;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * material.diffuse * diff;
    
    // Specular
    vec3 reflectDir = normalize(reflect(-lightDir, norm));    
    float spec = pow(max(dot(viewD, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color * material.specular * spec;
    
    // Применяем затухание
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateSpotLight(SpotLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    // --- Затухание ---
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // --- Конус (Spot Cutoff) ---
    float theta = dot(lightDir, normalize(-light.direction)); // Косинус угла между лучом и направлением прожектора
    float epsilon = light.cutOff - light.outerCutOff; // Разница для мягкого края
    
    // Расчет интенсивности конуса (0.0 вне, 1.0 внутри)
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Если Fragment находится вне внешнего конуса, свет не рассчитывается (интенсивность = 0)
    if (theta <= light.outerCutOff) {
        intensity = 0.0;
    }

    // Ambient, Diffuse, Specular (тот же расчет, что и для PointLight)
    vec3 ambient = light.color * material.ambient;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.color * material.diffuse * diff;
    vec3 reflectDir = normalize(reflect(-lightDir, norm));    
    float spec = pow(max(dot(viewD, reflectDir), 0.0), material.shininess);
    vec3 specular = light.color * material.specular * spec;
    
    // Применяем затухание И интенсивность конуса
    return (ambient + diffuse + specular) * attenuation * intensity;
}

// --- Главная функция ---
void main()
{    
    // Нормализация нормали (после интерполяции во фрагментном шейдере)
    vec3 norm = normalize(Normal);
    // Вектор взгляда (наблюдателя)
    vec3 viewD = normalize(viewPos - FragPos);
    
    // Цвет, полученный из текстуры
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);
    
    // Итоговый цвет освещения
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