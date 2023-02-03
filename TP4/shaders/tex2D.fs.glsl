#version 330 core

in vec2 vFragTextureCoord;
uniform vec3 uColor;
uniform sampler2D uTexture;

out vec3 fFragColor;


void main() {
    fFragColor = texture(uTexture, vFragTextureCoord).xyz;
};