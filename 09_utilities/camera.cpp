#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Camera::Camera(const vec3& position, const vec3& dir, const vec3& up, Window& window) :
    m_position(position), m_velocity(0.f, 0.f, 0.f),
    m_base_forward(normalize(dir)), m_base_up(normalize(up)), m_base_side(cross(m_base_up, m_base_forward)),
    m_forward(m_base_forward), m_side(m_base_side),
    m_current_rot_up(0.f), m_current_rot_sides(0.f),
    m_window(window)
{
    //set cursor as hidden and move it to the center of window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    centerCursor();
    computeViewMatrix();
}
void Camera::setViewMatrixDirectionOnly(bool is_direction_only){
    m_view_matrix_direction_only = is_direction_only;
}
void Camera::update(float dt){
    //adjust rotation based on how much mouse has moved
    glm::vec2 mouse_move = m_window.mousePos() - m_window.fsize() / 2.f;
    m_current_rot_sides -= mouse_move.x / 500;
    m_current_rot_up -= mouse_move.y / 500;
    //clamp rotation up to nearly directly upwards
    if (m_current_rot_up > PI/2 - 0.1) m_current_rot_up = PI/2 - 0.1;
    //clamp rotation up to nearly directly downwards
    if (m_current_rot_up < 0.1 - PI/2) m_current_rot_up = 0.1 - PI/2;
    //compute current direction from rotations. This is done by interpolating between right and front based on rot_sides, then interpolating between result and m_up based on rot_up
    m_forward = glm::normalize((sinf(m_current_rot_sides)*m_base_side+cosf(m_current_rot_sides)*m_base_forward)*cosf(m_current_rot_up) + sinf(m_current_rot_up)*m_base_up);
    m_side = glm::normalize(glm::cross(m_forward, m_base_up));




    //the following update velocity when keys are pressed. Directions used:
    // - glm::normalize(m_current_direction) is the direction which the camera is currently facing
    // - glm::normalize(glm::cross(m_current_direction, m_up)) is direction to the right (cross product of up and current direction)
    // - m_up - direction upwards



    if (m_window.keyOn(m_key_forward)){
        m_velocity += dt * m_acceleration * m_forward;
    }
    if (m_window.keyOn(m_key_backward)){
        m_velocity -= dt * m_acceleration * m_forward;
    }
    if (m_window.keyOn(m_key_right)){
        m_velocity += dt * m_acceleration * m_side;
    }
    if (m_window.keyOn(m_key_left)){
        m_velocity -= dt * m_acceleration * m_side;
    }
    if (m_window.keyOn(m_key_up)){
        m_velocity += dt * m_acceleration * m_base_up;
    }
    if (m_window.keyOn(m_key_down)){
        m_velocity -= dt * m_acceleration * m_base_up;
    }


    //add velocity to position
    m_position += m_velocity*dt;

    //damp velocity
    m_velocity *= 0.96f;

    centerCursor();
    computeViewMatrix();
}
const glm::vec3& Camera::getPosition() const{
    return m_position;
}
const float* Camera::getPositionPtr() const{
    return glm::value_ptr(m_position);
}
const glm::vec3& Camera::getDirection() const{
    return m_forward;
}
const float* Camera::getDirectionPtr() const{
    return glm::value_ptr(m_forward);
}

UniformBufferLayoutData& Camera::writePosition(UniformBufferLayoutData& data, const string& name){
    return data.write(name, getPositionPtr(), 3);
}
UniformBufferLayoutData& Camera::writeDirection(UniformBufferLayoutData& data, const string& name){
    return data.write(name, getDirectionPtr(), 3);
}


void Camera::centerCursor(){
    m_window.setMousePos(m_window.fsize() / 2.f);
}
void Camera::computeViewMatrix(){
    //if view matrix is direction only, create it by looking in given direction from (0, 0, 0)
    if (m_view_matrix_direction_only){
        view_matrix = glm::lookAt(vec3(0, 0, 0), m_forward, m_base_up);
    }else{
        //else use m_position to create view matrix
        view_matrix = glm::lookAt(m_position, m_position + m_forward, m_base_up);
    }
}






ProjectionSettings::ProjectionSettings(float ratio) : m_ratio(ratio) {}
ProjectionSettings::ProjectionSettings(const glm::uvec2& size) : ProjectionSettings(1.f * size.x / size.y) {}

ProjectionSettings& ProjectionSettings::setFOV(float fov){
    m_fov = glm::radians(fov);
    return *this;
}
ProjectionSettings& ProjectionSettings::setDistances(float near_plane_dist, float far_plane_dist){
    m_near = near_plane_dist;
    m_far = far_plane_dist;
    return *this;
}

float ProjectionSettings::getFOV() const {return m_fov;}
float ProjectionSettings::getRatio() const {return m_ratio;}
float ProjectionSettings::getNear() const {return m_near;}
float ProjectionSettings::getFar() const {return m_far;}

glm::mat4 ProjectionSettings::create(){
    //matrix to invert y axis - the one in vulkan is inverted compared to the one in OpenGL, for which GLM was written
    glm::mat4 invert_y_mat(1.0);
    invert_y_mat[1][1] = -1;
    return glm::perspective(m_fov, m_ratio, m_near, m_far) * invert_y_mat;
}



ProjectionCamera::ProjectionCamera(const vec3& position, const vec3& dir, const vec3& up, Window& window, ProjectionSettings projection_s) :
    Camera(position, dir, up, window), projection{projection_s.create()}
{}
void ProjectionCamera::update(float time_step){
    Camera::update(time_step);
    MVP = projection * view_matrix;
}
const glm::mat4& ProjectionCamera::getMVP() const{
    return MVP;
}
const float* ProjectionCamera::getMVPPtr() const{
    return glm::value_ptr(MVP);
}
UniformBufferLayoutData& ProjectionCamera::writeMVP(UniformBufferLayoutData& data, const string& name){
    return data.write(name, getMVPPtr(), 16);
}