#version 330 core

in vec3 vPosition_vs; // Position du sommet transform� dans l'espace View
in vec3 vNormal_vs;   // Normale du sommet transform� dans l'espace View
in vec2 vTexCoords;   // Coordonn�es de texture du sommet

uniform sampler2D uEarthTexture;
uniform sampler2D uCloudTexture;

out vec3 fFragColor;


void main() {
    fFragColor = texture(uEarthTexture, vTexCoords).xyz + texture(uCloudTexture, vTexCoords).xyz;
};