#include "window.h"

Window::Window(const VkInstance instance, uint32_t screen_width, uint32_t screen_height, string title) :
    m_width(screen_width), m_height(screen_height), m_vulkan_instance(instance)
{
    //initialize GLFW
    glfwInit();
    //create no default context, only OpenGL contexts are created this way
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //window will not be resizable
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //create window
    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
    //create vulkan surface for the window
    VkResult result = glfwCreateWindowSurface(m_vulkan_instance, m_window, nullptr, &m_screen_surface);
    DEBUG_CHECK("Create window surface", result);
}
void Window::update(){
    //process all events - keypresses, mouse, ...
    glfwPollEvents();
    //check whether the window isn't supposed to exit
    m_running = !(glfwWindowShouldClose(m_window) || keyOn(GLFW_KEY_ESCAPE));
}
bool Window::running(){
    return m_running;
}
bool Window::keyOn(int key){
    return glfwGetKey(m_window, key);
}
glm::vec2 Window::mousePos(){
    double x, y;
    glfwGetCursorPos(m_window, &x, &y);
    return glm::vec2(x, y);
}
void Window::setMousePos(const glm::vec2& v){
    glfwSetCursorPos(m_window, v.x, v.y);
}
const VkSurfaceKHR& Window::getSurface() const{
    return m_screen_surface;
}
Window::operator GLFWwindow*(){
    return m_window;
}
uint32_t Window::getWidth() const{
    return m_width;
}
uint32_t Window::getHeight() const{
    return m_height;
}
glm::vec2 Window::getSize() const
{
    return {m_width, m_height};
}
void Window::destroy(){
    //if screen surface was created, destroy it
    if (m_screen_surface){
        vkDestroySurfaceKHR(m_vulkan_instance, m_screen_surface, nullptr);
        m_screen_surface = VK_NULL_HANDLE;
    }
    //destroy the window and exit GLFW
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
