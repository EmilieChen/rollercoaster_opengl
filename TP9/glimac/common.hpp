#pragma once

#include <GLFW/glfw3.h>
#include "glm.hpp"

namespace glimac {

struct ShapeVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    ShapeVertex(glm::vec3 pos, glm::vec3 n, glm::vec2 uv)
    {
        position  = pos;
        normal    = n;
        texCoords = uv;
    }
};
} // namespace glimac
