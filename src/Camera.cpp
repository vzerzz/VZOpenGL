#include "Camera.h"
#include <iostream>

Camera::Camera(glm::vec3 position)
    : WorldUp(glm::vec3(0.0f, 1.0f, 0.0f)), Yaw(YAW), Pitch(PITCH), Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() const noexcept
{
    return glm::lookAt(Position, Position + Front, Up); //位置、目标和上向量
}

// 位置移动 更改Position
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) noexcept
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

// 鼠标控制视角移动
void Camera::ProcessMouseMovement(float xoffset, float yoffset) noexcept
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    // 把偏移量加到pitch和yaw上
    Yaw += xoffset;
    Pitch += yoffset;

    // 对俯仰角进行最大和最小值的限制 在90度时视角会发生逆转

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    // 通过俯仰角和偏航角来计算以得到真正的方向向量
    updateCameraVectors();
}

// 鼠标控制缩放
void Camera::ProcessMouseScroll(float yoffset) noexcept
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() noexcept
{
    // 通过俯仰角和偏航角来计算以得到真正的方向向量
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

float Camera::GetZoom() const noexcept{
    return Zoom;
}

glm::vec3 Camera::GetPosition() const noexcept{
    return Position;
}

glm::vec3 Camera::GetFront() const noexcept{
    return Front;
}