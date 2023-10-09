#version 330 core

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct MaterialWithTexture {
    sampler2D diffuse;
    sampler2D specular;
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
uniform MaterialWithTexture materialtex;
uniform bool withTexture;
uniform bool nightMode;
uniform bool exposedToSun;
uniform bool flashlightOn;
uniform bool ambientOn;
uniform bool diffuseOn;
uniform bool specularOn;
uniform vec3 emission;

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
    vec3 result;
    if (withTexture == true) result = dirLight.ambient * vec3(texture(materialtex.diffuse, TexCoords));
    else result = emission;
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
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);    
    float diff = max(dot(normal, lightDir), 0.0);    
    vec3 reflectDir = reflect(-lightDir, normal);    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    if (withTexture == true) {
        if (ambientOn == true) ambient = light.ambient * vec3(texture(materialtex.diffuse, TexCoords));
        if (diffuseOn == true) diffuse = light.diffuse * diff * vec3(texture(materialtex.diffuse, TexCoords));        
        if (specularOn == true) {
            float spectex = pow(max(dot(viewDir, reflectDir), 0.0), materialtex.shininess);
            specular = light.specular * spectex * vec3(texture(materialtex.specular, TexCoords));
        }
        
    }
    else {
        if (ambientOn == true) ambient = light.ambient * material.ambient;
        if (diffuseOn == true) diffuse = light.diffuse * (diff * material.diffuse);
        if (specularOn == true) {
            float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);        
            specular = light.specular * (spec * material.specular);
        }
    }
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    if (ambientOn == true) result += ambient;
    if (diffuseOn == true) result += diffuse;
    if (specularOn == true) result += specular;
    return result;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);    
    float diff = max(dot(normal, lightDir), 0.0f);    
    vec3 reflectDir = reflect(-lightDir, normal);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));   
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    if (withTexture == true) {
        if (ambientOn == true) ambient = light.ambient * vec3(texture(materialtex.diffuse, TexCoords));
        if (diffuseOn == true) diffuse = light.diffuse * diff * vec3(texture(materialtex.diffuse, TexCoords));        
        if (specularOn == true) {
            float spectex = pow(max(dot(viewDir, reflectDir), 0.0f), materialtex.shininess);
            specular = light.specular * spectex * vec3(texture(materialtex.specular, TexCoords));
        }         
    }
    else {
        if (ambientOn == true) ambient = light.ambient * material.ambient;
        if (diffuseOn == true) diffuse = light.diffuse * (diff * material.diffuse);
        if (specularOn == true) {
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            specular = light.specular * (spec * material.specular);
        }         
    }
    if (ambientOn == true) ambient *= attenuation;
    if (diffuseOn == true) diffuse *= attenuation;
    if (specularOn == true) specular *= attenuation;
    
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    if (ambientOn == true) result += ambient;
    if (diffuseOn == true) result += diffuse;
    if (specularOn == true) result += specular;
    return result;
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    if (withTexture == true) {
        if (ambientOn == true) ambient = light.ambient * vec3(texture(materialtex.diffuse, TexCoords));
        if (diffuseOn == true) diffuse = light.diffuse * diff * vec3(texture(materialtex.diffuse, TexCoords));        
        if (specularOn == true) {
            float spectex = pow(max(dot(viewDir, reflectDir), 0.0f), materialtex.shininess);
            specular = light.specular * spectex * vec3(texture(materialtex.specular, TexCoords));
        }        
    }
    else {
        if (ambientOn == true) ambient = light.ambient * material.ambient;
        if (diffuseOn == true) diffuse = light.diffuse * (diff * material.diffuse);
        if (specularOn == true) {
            float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
            specular = light.specular * (spec * material.specular);
        }        
    }
    if (ambientOn == true) ambient *= attenuation * intensity;
    if (diffuseOn == true) diffuse *= attenuation * intensity;
    if (specularOn == true) specular *= attenuation * intensity;

    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    if (ambientOn == true) result += ambient;
    if (diffuseOn == true) result += diffuse;
    if (specularOn == true) result += specular;
    return result;
}