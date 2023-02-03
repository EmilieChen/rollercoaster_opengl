#version 330

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
    float greyScale = (vFragColor.x + vFragColor.y + vFragColor.z) / 3;
    fFragColor = vec3(greyScale, greyScale, greyScale);
}
