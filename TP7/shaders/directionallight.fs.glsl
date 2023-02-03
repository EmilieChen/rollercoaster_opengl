#version 330 core

in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs;   // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords;   // Coordonnées de texture du sommet

out vec3 ffragColor;
// uniform sampler2D uEarthTexture;
//uniform sampler2D uCloudTexture;

uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;
uniform vec3  uLightDir_vs;
uniform vec3  uLightIntensity;

vec3 blinnPhong() {
    /*vec3 wi = normalize(uLightDir_vs);
    vec3 Li = uLightIntensity;
    vec3 wo = normalize(-vPosition_vs));
    vec3 halfVector = (wi + wo) / 2;*/

    return vec3(0.0, 0.0, 0.0); /*Li * ( uKd* dot(wi, vNormal_vs) + uKs * pow(dot(halfVector, vNormal_vs), uShininess) );*/
}

void main()
{
    fFragColor = blinnPhong();
}
