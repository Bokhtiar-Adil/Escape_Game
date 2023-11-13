#version 330 core

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_NR_POINT_LIGHTS 4
#define MAX_NR_STREET_LIGHTS 16

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform int numberofPointlights;
uniform vec3 viewPos;
uniform DirectionalLight dirLight;
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform bool pointLightStatus[MAX_NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform bool nightMode;
uniform bool exposedToSun;
uniform bool flashlightOn;
uniform vec3 emission;
uniform PointLight streetLights[MAX_NR_STREET_LIGHTS];
uniform bool streetLightStatus[MAX_NR_STREET_LIGHTS];
uniform int numberofStreetlights;
uniform bool streetLightOn;
uniform bool darkBonusAchieved;


// function prototypes
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    //emission = vec3(0.05f, 0.05f, 0.05f);
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = emission;
    // phase 1: directional lighting
    if (nightMode == false && exposedToSun == true) {
        result = CalcDirectionalLight(dirLight, norm, viewDir);
    }
    // phase 2: point lights
    for(int i = 0; i < numberofPointlights; i++)
        if (pointLightStatus[i] == true) {
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        }
    // phase 3: spot light
    if (flashlightOn == true) {
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    }
    // streetLights calculation - a variant of pointlights
    if (streetLightOn == true) {
        for(int i = 0; i < numberofStreetlights; i++) {
            if (streetLightStatus[i] == true) {
                result += CalcPointLight(streetLights[i], norm, FragPos, viewDir);
            }
        }
    }
    if (darkBonusAchieved == true) {
        result = CalcSpotLight(spotLight, norm, FragPos, viewDir);
    }
    FragColor = vec4(result, 1.0f);
}

// calculates the color when using a directional light.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);    
    float diff = max(dot(normal, lightDir), 0.0);    
    vec3 reflectDir = reflect(-lightDir, normal);    

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);        
    vec3 specular = light.specular * (spec * material.specular);
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);    
    float diff = max(dot(normal, lightDir), 0.0f);    
    vec3 reflectDir = reflect(-lightDir, normal);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 
    
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);        
    vec3 specular = light.specular * (spec * material.specular);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);    

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);        
    vec3 specular = light.specular * (spec * material.specular);

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}