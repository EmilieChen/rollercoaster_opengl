#version 330 core


struct Material {
    sampler2D Kdiffuse;
    vec3  Kspecular; // sampler2D specular;
    float     shininess;
};

struct Light {
    vec3 dir_pos;
    vec3 ambient;
    vec3 diffuse;
};

in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs;   // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords;   // Coordonnées de texture du sommet

#define NB_POINT_LIGHTS 2

uniform vec3       uViewPosition;
uniform Light   uDirLight;
uniform Light      uPointLights[NB_POINT_LIGHTS];
uniform Material   uMaterial;

out vec3 fFragColor;

// calculates the color when using a directional light.
vec3 CalcDirLight(Light light)
{
    vec3 ambient  = light.ambient * vec3(texture(uMaterial.Kdiffuse, vTexCoords));

    vec3 wi         = normalize(light.dir_pos);
    vec3 Li         = light.diffuse;
    vec3 wo         = normalize(-vPosition_vs);
    vec3 halfVector = normalize((wi + wo) / 2.0);

    vec3 diffuse_specular = Li * (vec3(texture(uMaterial.Kdiffuse, vTexCoords)) * dot(wi, vNormal_vs) + uMaterial.Kspecular * pow(dot(halfVector, vNormal_vs), uMaterial.shininess));
    
    return diffuse_specular  + ambient;
}

//calculates the color when using a point light.
vec3 CalcPointLight(Light light)
{
    float d           = distance(light.dir_pos, vPosition_vs);
    float attenuation = 1.0 / (1.0 + 0.005 * (d * d));

    vec3 ambient = light.ambient * vec3(texture(uMaterial.Kdiffuse, vTexCoords));

    vec3 wi         = normalize(light.dir_pos - vPosition_vs);
    vec3 Li         = light.diffuse;
    vec3 wo         = normalize(-vPosition_vs);
    vec3 halfVector = normalize((wi + wo) / 2.0);
    
    vec3 diffuse_specular = Li * (vec3(texture(uMaterial.Kdiffuse, vTexCoords)) * dot(wi, vNormal_vs) + uMaterial.Kspecular * pow(dot(halfVector, vNormal_vs), uMaterial.shininess));
    
    ambient *= attenuation;
    diffuse_specular *= attenuation;
    
    return diffuse_specular + ambient;
}

void main()
{
    // phase 1: directional lighting
    vec3 result = CalcDirLight(uDirLight);
    //vec3 result = vec3(0, 0, 0);
    // phase 2: point lights
    for (int i = 0; i < NB_POINT_LIGHTS; i++)
        result += CalcPointLight(uPointLights[i]);

    fFragColor = result;
}

