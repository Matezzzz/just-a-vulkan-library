#ifndef WINDOW_H
#define WINDOW_H

/**
 * window.h
 *  - Contains windows class
 */


#include "../00_base/vulkan_base.h"
#include "../01_device/vulkan_instance.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

/**
 * Window
 *  - Wrapper around GLFWwindow class, uses GLFW to create and manage window
 */
class Window{
public:
    const uint32_t width;
    const uint32_t height;
private:
    const VkInstance m_vulkan_instance;
    GLFWwindow* m_window;
    VkSurfaceKHR m_screen_surface = VK_NULL_HANDLE;
    //app is supposed to terminate if this is false
    bool m_running = true;
public:
    //Create window and initialize GLFW
    Window(const VkInstance instance, uint32_t width, uint32_t height, string title);

    //poll events and check whether window shouldn't be closed
    void update();

    //if false, app should close
    bool running();

    //check whether key with the given key code is pressed
    bool keyOn(int glfw_key_code);

    template<int button_code>
    bool mousePressed(){
        return glfwGetMouseButton(m_window, button_code);
    }
    bool mouseLeft();
    bool mouseRight();

    //return mouse position in the window
    glm::vec2 mousePos();

    //move mouse to the given coordinates
    void setMousePos(const glm::vec2& v);
    const VkSurfaceKHR& getSurface() const;
    operator GLFWwindow*();

    //get a (width, height) vector
    glm::uvec2 size() const;

    //get a (width, height) float vector. Both in pixels.
    glm::vec2 fsize() const;

    //destroy window and terminate GLFW
    void destroy();
};

#endif // WINDOW_H
