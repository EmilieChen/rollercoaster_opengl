#version 330

in vec3 vFragColor;
in vec2 vFragPosition;

out vec3 fFragColor;

void main()
{
    // float a    = length(fract(5.0 * vFragPosition));
    // float a        = length(abs(fract(5.0 * vFragPosition) * 2.0 - 1.0));
    // float a    = mod(floor(10.0 * vFragPosition.x) + floor(50.0 * vFragPosition.y), 2.0);
    // float a = smoothstep(0.3, 0.32, length(fract(5.0 * vFragPosition) - 0.5));
    // float a = smoothstep(0.4, 0.5, max(abs(fract(8.0 * vFragPosition.x - 0.5 * mod(floor(8.0 * vFragPosition.y), 2.0)) - 0.5), abs(fract(8.0 * vFragPosition.y) - 0.5)));
    // float a = max( smoothstep(0.4, 0.5, length(fract(5.0 * vFragPosition) - 0.5)), mod(floor(10.0 * vFragPosition.x) + floor(50.0 * vFragPosition.y), 2.0));
    // float a    = length(smoothstep(0.2, 0.7, max(abs(fract(6.0 * vFragPosition.x - 0.5 * mod(floor(8.0 * vFragPosition.x), 2.0)) - 0.5), abs(fract(8.0 * vFragPosition.y) - 0.5))) * 2);
    // float a = distance( smoothstep(0.4, 0.5, length(fract(5.0 * vFragPosition) - 0.5)), mod(floor(10.0 * vFragPosition.x) + floor(50.0 * vFragPosition.y), 2.0));
    float a = distance(smoothstep(0.4, 0.5, length(fract(5.0 * vFragPosition) - 0.5)), smoothstep(0.3, 0.33, length(fract(5.0 * vFragPosition) - 0.5)));

    fFragColor = a * vFragColor;
}
+