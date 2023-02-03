#pragma once

#include <vector>
#include <memory>
#include <unordered_map>

#include "glm.hpp"
#include "FilePath.hpp"

namespace glimac {

class FreelyCamera {
private:
    glm::vec3 m_Position;
    float     m_fPhi;
    float     m_fTheta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

public:
    FreelyCamera()
    {
        this->m_Position = glm::vec3(0, 0, 5);
        this->m_fPhi     = glm::pi<float>();
        this->m_fTheta   = 0;
        computeDirectionVectors();
    };

    void moveLeft(float t)
    {
        this->m_Position += t*this->m_LeftVector;
    };
    
    void moveFront(float t)
    {
        this->m_Position += t * this->m_FrontVector;
    };

    void rotateLeft(float degrees)
    {
        this->m_fPhi += glm::radians(degrees);
    };

    void rotateUp(float degrees)
    {
        this->m_fTheta += glm::radians(degrees);
    };

    void computeDirectionVectors() 
    {
        this->m_FrontVector = glm::vec3(cos(this->m_fTheta) * sin(this->m_fPhi), sin(this->m_fTheta), cos(this->m_fTheta) * cos(this->m_fPhi)); 
        this->m_LeftVector  = glm::vec3(sin(this->m_fPhi + glm::pi<float>() / 2), 0, cos(this->m_fPhi + glm::pi<float>() / 2) );
        this->m_UpVector    = glm::cross(this->m_FrontVector, this->m_LeftVector);
    }
    
    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(this->m_Position, this->m_Position+this->m_FrontVector, this->m_UpVector);
    }

    glm::vec3 getCameraPosition() const
    {
        return this->m_Position;
    }
};
}