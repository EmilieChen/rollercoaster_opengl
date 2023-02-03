#version 330 core

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;

vec2 complexSqr(vec2 z) {
    return vec2(pow(z.x, 2) - pow(z.y, 2), 2 * z.x * z.y);
}


void main()
{
    vec2 position = 2 * vFragPosition;
    float Nmax = 10000f;
    for (float i = 0; i < Nmax; i++) {
        position = complexSqr(position) + vFragPosition;
        if (length(position) > 2) {
            fFragColor = vec3((Nmax-i-1) / Nmax);
            return;
        }
    }
    fFragColor = vec3(0, 0, 0);
    //fFragColor = ((vFragPosition.x + 1)/2) * vec3(1, 0, 0) + ((vFragPosition.y+1)/2) * vec3(0, 1, 0);
}
