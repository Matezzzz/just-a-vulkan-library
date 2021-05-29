#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(vec3 position, vec3 dir, vec3 up, Window& window) :
    m_position(position), m_velocity(0.f, 0.f, 0.f), m_current_direction(normalize(dir)), m_direction_forward(normalize(dir)),
    m_up(normalize(up)), m_direction_side(cross(m_up, m_direction_forward)),
    m_current_rot_up(0.f), m_current_rot_sides(0.f), m_window(window), view_matrix(glm::lookAt(m_position, m_position + m_current_direction, m_up))
{
    //set cursor as hidden and move it to the center of window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    centerCursor();
}
void Camera::setViewMatrixDirectionOnly(bool is_direction_only){
    m_view_matrix_direction_only = is_direction_only;
}
void Camera::update(float dt){
    //the following update velocity when keys are pressed. Directions used:
    // - glm::normalize(m_current_direction) is the direction which the camera is currently facing
    // - glm::normalize(glm::cross(m_current_direction, m_up)) is direction to the right (cross product of up and current direction)
    // - m_up - direction upwards

    if (m_window.keyOn(m_key_forward)){
        m_velocity += dt * m_acceleration * glm::normalize(m_current_direction);
    }
    if (m_window.keyOn(m_key_backward)){
        m_velocity -= dt * m_acceleration * glm::normalize(m_current_direction);
    }
    if (m_window.keyOn(m_key_right)){
        m_velocity += dt * m_acceleration * glm::normalize(glm::cross(m_current_direction, m_up));
    }
    if (m_window.keyOn(m_key_left)){
        m_velocity -= dt * m_acceleration * glm::normalize(glm::cross(m_current_direction, m_up));
    }
    if (m_window.keyOn(m_key_up)){
        m_velocity += dt * m_acceleration * m_up;
    }
    if (m_window.keyOn(m_key_down)){
        m_velocity -= dt * m_acceleration * m_up;
    }

    //adjust rotation based on how much mouse has moved
    glm::vec2 mouse_move = m_window.mousePos() - m_window.getSize() / 2.f;
    m_current_rot_sides -= mouse_move.x / 500;
    m_current_rot_up -= mouse_move.y / 500;
    //clamp rotation up to nearly directly upwards
    if (m_current_rot_up > PI/2 - 0.1) m_current_rot_up = PI/2 - 0.1;
    //clamp rotation up to nearly directly downwards
    if (m_current_rot_up < 0.1 - PI/2) m_current_rot_up = 0.1 - PI/2;
    //compute current direction from rotations. This is done by interpolating between right and front based on rot_sides, then interpolating between result and m_up based on rot_up
    m_current_direction = (sinf(m_current_rot_sides)*m_direction_side+cosf(m_current_rot_sides)*m_direction_forward)*cosf(m_current_rot_up) + sinf(m_current_rot_up)*m_up;

    //add velocity to position
    m_position += m_velocity*dt;
    //damp velocity
    m_velocity *= 0.96f;
    //if view matrix is direction only, create it by looking in given direction from (0, 0, 0)
    if (m_view_matrix_direction_only){
        view_matrix = glm::lookAt(vec3(0, 0, 0), m_current_direction, m_up);
    }else{
        //else use m_position to create view matrix
        view_matrix = glm::lookAt(m_position, m_position + m_current_direction, m_up);
    }
    centerCursor();
}
const glm::vec3& Camera::getPosition() const{
    return m_position;
}
const glm::vec3& Camera::getDirection() const{
    return m_current_direction;
}
void Camera::centerCursor(){
    m_window.setMousePos(m_window.getSize() / 2.f);
}