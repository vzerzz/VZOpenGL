#pragma once

#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// options for movement
enum Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera{
public:
    explicit Camera(glm::vec3 position);
    explicit Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    
    glm::mat4 GetViewMatrix() const noexcept;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) noexcept;

    void ProcessMouseMovement(float xoffset, float yoffset) noexcept;

    void ProcessMouseScroll(float yoffset) noexcept;

    float GetZoom() const noexcept;

    glm::vec3 GetPosition() const noexcept;
    glm::vec3 GetFront() const noexcept;

private:
    void updateCameraVectors() noexcept;
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;//偏航角
    float Pitch;//俯仰角
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

};