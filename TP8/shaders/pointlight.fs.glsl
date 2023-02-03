#version 330 core

in vec3 vPosition_vs; // Position du sommet transform� dans l'espace View
in vec3 vNormal_vs;   // Normale du sommet transform� dans l'espace View
in vec2 vTexCoords;   // Coordonn�es de texture du sommet

out vec3 fFragColor;
// uniform sampler2D uEarthTexture;
// uniform sampler2D uCloudTexture;

uniform vec3  uKd;
uniform vec3  uKs;
uniform float uShininess;
uniform vec3  uLightPos_vs;
uniform vec3  uLightIntensity;

vec3 blinnPhong()
{
    vec3 wi = normalize(uLightPos_vs - vPosition_vs);
    float d          = distance(uLightPos_vs, vPosition_vs);
    vec3 Li = uLightIntensity / (d * d);
    vec3 wo = normalize(-vPosition_vs);
    vec3 halfVector = (wi + wo) / 2.0;

    return Li*(uKd * dot(wi, vNormal_vs) + uKs * pow(dot(halfVector, vNormal_vs), uShininess));
}

void main()
{
    fFragColor = blinnPhong();
}
