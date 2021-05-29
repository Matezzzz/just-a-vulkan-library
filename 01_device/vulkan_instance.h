#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

/**
 * vulkan_instance.h
 *  - Classes related to loading vulkan library from dll and creating vulkan instance
 */

#include "../00_base/vulkan_base.h"
#include "../00_base/extension_utilities.h"

#include <iostream>
#include <vector>

using std::cout;
using std::vector;

class Device;
class Window;
class VulkanInstance;




/**
 * VulkanInstanceCreateInfo
 *  - Holds all information for creating a vulkan instance
 */
class VulkanInstanceCreateInfo{
    string m_app_name = "Vulkan app";
    string m_engine_name = "Vulkan engine";
    VkApplicationInfo m_app_info;
    vector<string> m_extensions;
public:
    //Create new instance info with name 'Vulkan app', engine name 'Vulkan engine', all versions as '1.0.0' and no extensions
    VulkanInstanceCreateInfo(); 
    VulkanInstanceCreateInfo& appName(const string& name);
    VulkanInstanceCreateInfo& appVersion(int major, int minor, int patch);
    VulkanInstanceCreateInfo& engineName(const string& name);
    VulkanInstanceCreateInfo& engineVersion(int major, int minor, int patch);
    VulkanInstanceCreateInfo& apiVersion(int major, int minor, int patch);
    VulkanInstanceCreateInfo& requestExtensions(const vector<string>& desired_extensions);
    //Create instance and load instance level functions
    VkInstance create() const;
};



/**
 * VulkanLibrary
 *  - Loads library from dll and loads all global level functions
 *  - Is responsible for deleting the created instance
 */
class VulkanLibrary
{
    HMODULE m_library;
    VulkanInstance* m_instance = nullptr;
public:
    //load library and global functions
    VulkanLibrary();
    VulkanInstance& createInstance(const VulkanInstanceCreateInfo& info);
    //destroy all instances, then unload library
    ~VulkanLibrary();
};





/**
 * VulkanInstance
 *  - Holds one vulkan instance, and the device and windows created on it
 */
class VulkanInstance{
    VkInstance m_instance;

    Device* m_logical_device = nullptr;
    Window* m_window = nullptr;
public:
    VulkanInstance(VkInstance instance);

    const VkInstance& get();
    operator VkInstance();
    //Destroy device, all windows and instance
    void destroy();

    //Create logical device with this instance
    Device& createLogicalDevice(const VkDeviceCreateInfo& device_create_info, VkPhysicalDevice physical_device, const vector<VkDeviceQueueCreateInfo>& queue_infos, const vector<const char*>& extensions);
    //Create a new window with this instance
    Window& createWindow(uint32_t screen_width, uint32_t screen_height, const string& window_name);
};

#endif