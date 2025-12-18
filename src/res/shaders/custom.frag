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
    float shininess; // Будет использоваться как Roughness (шероховатость)
    sampler2D texture_diffuse1; 
};
uniform Material material;

// --- Функция для расчета Oren-Nayar Diffuse ---
vec3 calculateOrenNayarDiffuse(vec3 lightDir, vec3 lightColor, vec3 fragNormal, float roughness) {
    // Lambertian Diffuse (Стандартный Phong)
    float diff = max(dot(fragNormal, lightDir), 0.0);

    // Модель Oren-Nayar:
    
    // Шероховатость (alpha) от 0 до 1. 
    // Предполагаем, что material.shininess (0..100) обратно пропорционален roughness (0..1)
    // float alpha = 1.0 - clamp(roughness / 100.0, 0.0, 1.0); // Пример
    float alpha = roughness; // Если roughness уже в диапазоне [0, 1]
    
    float alphaSq = alpha * alpha;
    
    float A = 1.0 - 0.5 * alphaSq / (alphaSq + 0.33);
    float B = 0.45 * alphaSq / (alphaSq + 0.09);

    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Углы для Oren-Nayar
    float cos_theta_i = dot(lightDir, fragNormal);
    float cos_theta_r = dot(viewDir, fragNormal);
    
    // Sin(alpha) and Tan(alpha)
    float sin_alpha, tan_alpha;
    if (cos_theta_i > cos_theta_r) {
        sin_alpha = length(lightDir - fragNormal * cos_theta_i);
        tan_alpha = sin_alpha / cos_theta_i;
    } else {
        sin_alpha = length(viewDir - fragNormal * cos_theta_r);
        tan_alpha = sin_alpha / cos_theta_r;
    }

    // Cos(phi_diff)
    vec3 v_perp = normalize(viewDir - fragNormal * cos_theta_r);
    vec3 l_perp = normalize(lightDir - fragNormal * cos_theta_i);
    float cos_phi_diff = dot(v_perp, l_perp);
    
    // Финальная формула Oren-Nayar
    float L_on = max(0.0, cos_theta_i) * (A + B * max(0.0, cos_phi_diff) * sin_alpha * tan_alpha);

    // Смешиваем результат с обычным диффузным (для ясности)
    // Можно использовать только L_on
    return lightColor * material.diffuse * L_on;
}

// --- Функции для отдельных типов света ---

vec3 calculateDirLight(DirLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(-light.direction);
    
    // 1. Ambient
    vec3 ambient = light.color * material.ambient * light.ambientIntensity;
    
    // 2. Diffuse (Oren-Nayar)
    vec3 diffuse = calculateOrenNayarDiffuse(lightDir, light.color, norm, material.shininess);
    
    // 3. Specular (Используем стандартный Phong блик, так как Oren-Nayar только для diffuse)
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewD, reflectDir), 0.0), 32.0); // Фиксированный specular
    vec3 specular = light.color * material.specular * spec;
    
    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    // Затухание
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Diffuse (Oren-Nayar)
    vec3 diffuse = calculateOrenNayarDiffuse(lightDir, light.color, norm, material.shininess);
    
    // Specular (Phong)
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewD, reflectDir), 0.0), 32.0);
    vec3 specular = light.color * material.specular * spec;
    
    // Ambient
    vec3 ambient = light.color * material.ambient;

    return (ambient + diffuse + specular) * attenuation;
}

// calculateSpotLight будет аналогичен PointLight, но с добавлением конуса.
vec3 calculateSpotLight(SpotLight light, vec3 norm, vec3 viewD) {
    vec3 lightDir = normalize(light.position - FragPos);
    float distance = length(light.position - FragPos);
    
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // Конус
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    if (theta <= light.outerCutOff) intensity = 0.0;
    
    // Diffuse (Oren-Nayar)
    vec3 diffuse = calculateOrenNayarDiffuse(lightDir, light.color, norm, material.shininess);
    
    // Specular (Phong)
    vec3 reflectDir = normalize(reflect(-lightDir, norm));  
    float spec = pow(max(dot(viewD, reflectDir), 0.0), 32.0);
    vec3 specular = light.color * material.specular * spec;
    
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