#include "vulkan_instance.h"
#include "../02_swapchain/window.h"
#include "device.h"


VulkanInstanceCreateInfo::VulkanInstanceCreateInfo() :
    m_app_info{VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, m_app_name.c_str(), VK_MAKE_VERSION(1, 0, 0), m_engine_name.c_str(), VK_MAKE_VERSION(1, 0, 0), VULKAN_VERSION}
{}

VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::appName(const string& name){
    //save app name, then change pointer in structure
    m_app_name = name;
    m_app_info.pApplicationName = m_app_name.c_str();
    return *this;
}
VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::appVersion(int major, int minor, int patch){
    m_app_info.applicationVersion = VK_MAKE_VERSION(major, minor, patch);
    return *this;
}
VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::engineName(const string& name){
    //save engine name, then change pointer in structure
    m_engine_name = name;
    m_app_info.pEngineName = m_engine_name.c_str();
    return *this;
}
VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::engineVersion(int major, int minor, int patch){
    m_app_info.engineVersion = VK_MAKE_VERSION(major, minor, patch);
    return *this;
}
VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::apiVersion(int major, int minor, int patch){
    m_app_info.apiVersion = VK_MAKE_VERSION(major, minor, patch);
    return *this;
}
VulkanInstanceCreateInfo& VulkanInstanceCreateInfo::requestExtensions(const vector<string>& desired_extensions){
    //load all available extensions
    InstanceExtensionList instance_extensions;
    //save all extensions that are both available and requested
    instance_extensions.extensionsAvailable(desired_extensions, m_extensions);
    return *this;
}
VkInstance VulkanInstanceCreateInfo::create() const{
    //convert extensions to const char* strings
    vector<const char*> extensions = convertToVectorOfPointers(m_extensions);
    //fill instance create info
    VkInstanceCreateInfo create_info{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &m_app_info, 0, nullptr, (uint32_t) extensions.size(), extensions.data()};

    //create instance
    VkInstance instance;
    VkResult result = vkCreateInstance(&create_info, nullptr, &instance);
    DEBUG_PRINT("Create instance", result)

    //load instance functions
    int result2 = loadInstanceLevelFunctions(instance, extensions);
    DEBUG_CHECK("Load instance functions", result2)

    return instance;
}







VulkanLibrary::VulkanLibrary(){
    //load library dll
    m_library = LoadLibrary("vulkan-1.dll");
    DEBUG_PRINT_INV("Load vulkan library", m_library);
    //load global vulkan functions
    int result = loadVulkanFunctions(m_library);
    DEBUG_CHECK("Load vulkan functions", result)
}

VulkanInstance& VulkanLibrary::createInstance(const VulkanInstanceCreateInfo& info){
    //if instance exists already, print error, otherwise, create it
    DEBUG_CHECK("Instance already exists", m_instance)
    m_instance = new VulkanInstance(info.create());
    return *m_instance;
}

VulkanLibrary::~VulkanLibrary(){
    //destroy instance if it has been created
    if (m_instance){
        m_instance->destroy();
        delete m_instance;
    }
    //if library was loaded, free it
    if (m_library){
        FreeLibrary(m_library);
        m_library = nullptr;
    }
}







VulkanInstance::VulkanInstance(VkInstance instance) : m_instance(instance){}
Device& VulkanInstance::createLogicalDevice(const VkDeviceCreateInfo& device_create_info, VkPhysicalDevice physical_device, const vector<VkDeviceQueueCreateInfo>& queue_infos, const vector<const char*>& extensions)
{
    //create device
    VkDevice device;
    int result = vkCreateDevice( physical_device, &device_create_info, nullptr, &device);
    DEBUG_PRINT("Logical device creation", result)
    
    //load all device level functions
    result = loadDeviceLevelFunctions(device, extensions);
    DEBUG_CHECK("Device function load", result)

    //create logical device object using created handle
    DEBUG_CHECK("Device was already created", m_logical_device)
    m_logical_device = new Device(device, physical_device, queue_infos);
    return *m_logical_device;
}
Window& VulkanInstance::createWindow(uint32_t screen_width, uint32_t screen_height, const string& window_name){
    //create window with given parameters
    DEBUG_CHECK("Window was already created", m_window)
    m_window = new Window(m_instance, screen_width, screen_height, window_name);
    return *m_window;
}
const VkInstance& VulkanInstance::get() {return m_instance;}
VulkanInstance::operator VkInstance() {return m_instance;}


void VulkanInstance::destroy()
{
    //delete logical device if it has been created
    if (m_logical_device){
        m_logical_device->destroy();
        delete m_logical_device;
    }
    //delete window if it has been created
    if (m_window){
        m_window->destroy();
        delete m_window;
    }
    //delete instance if it has been created
    if (m_instance){
        vkDestroyInstance(m_instance, nullptr);
        m_instance = VK_NULL_HANDLE;
    }
}
