#version 330

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;

void main() {

    float a    = 2*exp(-50 * pow(distance(vFragPosition, vec2(0)), 2) );
    fFragColor = a * vFragColor;
}
