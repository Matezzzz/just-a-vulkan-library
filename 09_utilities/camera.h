#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "../02_swapchain/window.h"

#define PI 3.1415926

using glm::vec3;


/**
 * Camera
 *  - Holds camera, that is able to move using input from keyboard and generates a view matrix
 */
class Camera{
    vec3 m_position;
    vec3 m_velocity;
    //the direction, in which user is looking
    vec3 m_current_direction;
    //direction forward when creating camera
    const vec3 m_direction_forward;
    //vector pointing upwards
    const vec3 m_up;
    //vector pointing sideways
    const vec3 m_direction_side;

    //current rotation around current side direction
    float m_current_rot_up;
    //current rotation around up axis
    float m_current_rot_sides;

    //window, from which camera reads key presses
    Window& m_window;

    //When true, camera position will not be considered for creating the view matrix. This is useful when generating ray tracing vectors.
    bool m_view_matrix_direction_only = false;

    //controls
    const int m_key_forward  = GLFW_KEY_W;
    const int m_key_backward = GLFW_KEY_S;
    const int m_key_right    = GLFW_KEY_D;
    const int m_key_left     = GLFW_KEY_A;
    const int m_key_up = GLFW_KEY_SPACE;
    const int m_key_down = GLFW_KEY_LEFT_SHIFT;
    const float m_acceleration = 20.f;
public:
    //the camera matrix, is recreated every frame
    glm::mat4 view_matrix;
    //create camera in given position, with given direction and up vector and window. Constant vectors are set as forward(dir), up(up) and side(cross(dir, up))
    Camera(vec3 position, vec3 dir, vec3 up, Window& window);
    //When set as true, camera position will not be considered for creating the view matrix. This is useful when generating ray tracing vectors.
    void setViewMatrixDirectionOnly(bool is_direction_only);
    //read key presses, update camera position and rotation
    void update(float time_step);
    const glm::vec3& getPosition() const;
    const glm::vec3& getDirection() const;
private:
    void centerCursor();
};

#endif