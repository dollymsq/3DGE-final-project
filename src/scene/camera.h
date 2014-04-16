#ifndef CAMERA_H
#define CAMERA_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include <QDebug>

class Camera
{
public:
    Camera();

    glm::mat4 pMatrix; // Projection matrix
    glm::mat4 vMatrix; // View  matrix

    bool pressingForward;
    bool pressingBackward;
    bool pressingLeft;
    bool pressingRight;

    // TODO: move player properties to a player class

    bool pressingJump;

    void setAspectRatio(float aspect);
    void orientLook(glm::vec3 position, glm::vec3 look, glm::vec3 up);
    void mouseRotation(glm::vec2 delta);
    void update(float seconds);

    glm::vec3 m_lookAt;
    glm::vec3 m_position;
private:
    glm::vec3 m_up;

    glm::vec2 m_rotation;
    glm::vec2 m_lastRotation;

    glm::vec3 m_velocity;

    float m_fov;

    void updatePerspectiveMatrix();

    float m_speed;
    float m_pitchSpeed; // up and down
    float m_yawSpeed;  // left and right
};

#endif // CAMERA_H
