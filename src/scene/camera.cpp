#include "camera.h"

Camera::Camera()
    : m_lookAt(0.25f, 0.25f, 0.25f)
    , m_up(0.0f, 1.0f, 0.0f)
    , m_velocity(0.0f)
    , m_fov(1.047f)
    , m_speed(25.0f)
    , m_pitchSpeed(0.6f)
    , m_yawSpeed(0.6f)
{
    pressingJump =
    pressingForward =
    pressingBackward =
    pressingLeft =
    pressingRight = false;

    pMatrix = glm::perspective(m_fov, 4.0f / 3.0f, 0.1f, 100.f);
    vMatrix = glm::mat4(1.0f);

//    m_position = glm::vec3(5.1f, 3.5f, 5.1f);
    m_position = glm::vec3(50, 50, 50);
    m_rotation = m_lastRotation = glm::vec2(-126.299934, -21.89);
}

void Camera::setAspectRatio(float aspect)
{
    pMatrix = glm::perspective(m_fov, aspect, 0.1f, 1000.f);
}

void Camera::updatePerspectiveMatrix()
{
//    pMatrix = glm::perspective(m_fov, m_aspect, m_near, m_far);
}

void Camera::mouseRotation(glm::vec2 delta)
{
    delta = glm::clamp(delta, -50.0f, 50.0f);

    m_rotation.x += m_yawSpeed * delta.x;
    m_rotation.y += m_pitchSpeed * -delta.y;

//    if (m_rotation.x < 0) m_rotation.x += 360;
//    if (m_rotation.x >  360) m_rotation.x += -360;

    if (m_rotation.y < -90) m_rotation.y = -89.9f;
    if (m_rotation.y >  90) m_rotation.y =  89.9f;
}

void Camera::update(float seconds)
{
    glm::vec2 smoothRotation = glm::mix(m_lastRotation, m_rotation, glm::min(20.0f * seconds, 1.0f));
    m_lastRotation = smoothRotation;

    float rotX = glm::radians(smoothRotation.x);
    float rotY = glm::radians(smoothRotation.y);

    m_lookAt.x = glm::cos(rotY) * glm::cos(rotX);
    m_lookAt.z = glm::cos(rotY) * glm::sin(rotX);
    m_lookAt.y = glm::sin(rotY);

    if (pressingForward) {
        m_position += m_speed * seconds * glm::normalize(m_lookAt);
    }
    if (pressingBackward) {
        m_position -= m_speed * seconds * glm::normalize(m_lookAt);
    }
    if (pressingLeft | pressingRight){
        glm::vec3 cross = glm::normalize(glm::cross(m_lookAt, m_up));
        if (pressingLeft) m_position -= m_speed * seconds * cross;
        if (pressingRight) m_position += m_speed * seconds * cross;
    }

    m_lookAt += m_position;

    vMatrix = glm::lookAt(m_position, m_lookAt, m_up);
}
