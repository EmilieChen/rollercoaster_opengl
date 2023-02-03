#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "glm.hpp"
#include "FilePath.hpp"

namespace glimac {

class TrackballCamera {
private:
    float m_fDistance;
    float m_fAngleX;
    float m_fAngleY;

public:
    TrackballCamera()
    {
        this->m_fDistance = -5.0f;
        this->m_fAngleX   = 0.0f;
        this->m_fAngleY   = 0.0f;
    };
    void moveFront(float delta)
    {
        this->m_fDistance = this->m_fDistance + delta;
    };
    void rotateLeft(float degrees)
    {
        this->m_fAngleY = this->m_fAngleY + degrees;
    };
    void rotateUp(float degrees)
    {
        this->m_fAngleX = this->m_fAngleX + degrees;
    };
    glm::mat4 getViewMatrix() const {
        glm::mat4 MVMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, this->m_fDistance));
        MVMatrix           = glm::rotate(MVMatrix, glm::radians(this->m_fAngleX), glm::vec3(1, 0, 0));
        MVMatrix           = glm::rotate(MVMatrix, glm::radians(this->m_fAngleY), glm::vec3(0, 1, 0));
        return MVMatrix;
    }
};
}