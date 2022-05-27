#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include "../02_swapchain/window.h"
#include "../04_memory_objects/mixed_buffer.h"

#define PI 3.1415926

using glm::vec3;


/**
 * Camera
 *  - Holds camera, that is able to move using input from keyboard and generates a view matrix
 */
class Camera{
protected:
    vec3 m_position;
    vec3 m_velocity;

    //Forward, up, side during camera creation.
    const vec3 m_base_forward;
    const vec3 m_base_up;
    const vec3 m_base_side;

    //the direction, in which user is looking now
    vec3 m_forward;
    vec3 m_side;
    
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

    /**
     * @brief Construct a new camera with given position, direction and up vector
     * 
     * @param position starting position
     * @param dir initial looking direction
     * @param up intial up vector
     * @param window window to check for inputs
     */
    Camera(const vec3& position, const vec3& dir, const vec3& up, Window& window);
   
    //When set as true, camera position will not be considered for creating the view matrix. This is useful when generating ray tracing vectors.
    void setViewMatrixDirectionOnly(bool is_direction_only);
    
    //read key presses, update camera position and rotation
    void update(float time_step);

    //return a reference to the current camera position
    const glm::vec3& getPosition() const;

    //return a pointer to 3-element camera position float array, [x, y, z]
    const float* getPositionPtr() const;

    //return a reference to the current camera direction
    const glm::vec3& getDirection() const;

    //return a pointer to 3-element camera direction float array, [x, y, z]
    const float* getDirectionPtr() const;

    //call the write function on data, with params (name = name, data = getPositionPtr(), size = 3). Default name is 'camera_pos'
    UniformBufferLayoutData& writePosition(UniformBufferLayoutData& data, const string& name = "camera_pos");

    //call the write function on data, with params (name = name, data = getDirectionPtr(), size = 3). Default name is 'camera_dir'
    UniformBufferLayoutData& writeDirection(UniformBufferLayoutData& data, const string& name = "camera_dir");
private:
    //move cursor to the middle of the window
    void centerCursor();

    //compute view matrix from camera properties
    void computeViewMatrix();
};


//Holds settings for creating a projection matrix
class ProjectionSettings{
    float m_fov = glm::radians(45.f);
    float m_ratio = 1.f;
    float m_near = 0.1f;
    float m_far = 100.f;
public:
    /**
     * @brief Create projection settings with defaults - FOV(45°), given ratio, near(0.1f), far(100.f)
     * 
     * @param ratio width / height of view window
     */
    ProjectionSettings(float ratio = 1.f);

    /**
     * @brief Create projection settings with defaults - FOV(45°), ratio of given viewport, near(0.1f), far(100.f)
     * 
     * @param window_size ratio = width / height
     */
    ProjectionSettings(const glm::uvec2& window_size);
    
    //Set FOV (field of view) to the given value in degrees
    ProjectionSettings& setFOV(float fov_degrees);

    //Set near and far camera distances
    ProjectionSettings& setDistances(float near_plane_dist, float far_plane_dist);

    //get field of view
    float getFOV() const;

    //get width / height ratio
    float getRatio() const;

    //get near plane distance
    float getNear() const;

    //get far plane distance
    float getFar() const;

    //create a matrix using settings set until now
    glm::mat4 create();
};



//Camera that manages a projection in addition to the view matrix
class ProjectionCamera : public Camera{
protected:
    //projection matrix
    glm::mat4 projection;
    //model view projection matrix, (just view-projection for now)
    glm::mat4 MVP;
public:
    /**
     * @brief Construct a new Projection Camera object
     * 
     * @param position starting position
     * @param dir initial direction
     * @param up up vector
     * @param window window to get inputs from
     * @param projection_s settings defining the projection matrix to be used with this camera
     */
    ProjectionCamera(const vec3& position, const vec3& dir, const vec3& up, Window& window, ProjectionSettings projection_s = ProjectionSettings());
    
    //Move the camera, update view and MVP matrices
    void update(float time_step);

    //get the MVP matrix
    const glm::mat4& getMVP() const;

    //get a pointer to 16-element array containing MVP values
    const float* getMVPPtr() const;

    //call the write function on data, with params (name = name, data = getMVPPtr(), size = 16). Default name is 'MVP'
    UniformBufferLayoutData& writeMVP(UniformBufferLayoutData& data, const string& name = "MVP");
};
#endif