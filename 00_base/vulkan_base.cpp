#include "vulkan_base.h"

#include <windows.h>

#include "extension_utilities.h"

//console handle, used for colored output
HANDLE hConsole;

#define EXPORTED_VULKAN_FUNCTION( name ) PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) PFN_##name name;

#include "vulkan_function_list.inl"


//Load global vulkan functions from library
int loadVulkanFunctions(HMODULE& vulkan_library){
    //load handle to console
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    //define macros to automatically load all global functions
#define EXPORTED_VULKAN_FUNCTION( name )                                                    \
    name = reinterpret_cast<PFN_##name>((void*) GetProcAddress( vulkan_library, #name ));   \
    DEBUG_CHECK_INV("Could not load exported Vulkan function named: "#name, name)

#define GLOBAL_LEVEL_VULKAN_FUNCTION( name )                        \
    name = (PFN_##name)vkGetInstanceProcAddr( nullptr, #name );     \
    DEBUG_CHECK_INV("Could not load global level Vulkan function named: "#name, name)

    //by including function list with the macros specified above, this function will load all global vulkan functions
    #include "vulkan_function_list.inl"
    return 0;
}

//Load vulkan functions for given instance
int loadInstanceLevelFunctions(VkInstance& instance, const vector<const char*>& extensions){
    //define macros to automatically load all instance functions
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name )                      \
    name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );    \
    DEBUG_CHECK_INV("Could not load instance level Vulkan function named: "#name, name)

    //before loading the extension function, this macro checks that the extension to which the function belongs is available and enabled
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )    \
    for( const char* enabled_extension : extensions ){                      \
        if( !strcmp( enabled_extension, extension ) ){                      \
            name = (PFN_##name)vkGetInstanceProcAddr( instance, #name );    \
            DEBUG_CHECK_INV("Could not load extension instance level Vulkan function named: "#name, name)\
        }\
    }
    #include "vulkan_function_list.inl"
    return 0;
}

//Load vulkan functions for given device
int loadDeviceLevelFunctions(VkDevice& device, const vector<const char*>& extensions){
    //define macros to automatically load all device functions
#define DEVICE_LEVEL_VULKAN_FUNCTION( name )                        \
    name = (PFN_##name)vkGetDeviceProcAddr( device, #name );        \
    DEBUG_CHECK_INV("Could not load device-level Vulkan function named: "#name, name)

    //before loading the extension function, this macro checks that the extension to which the function belongs is available and enabled
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension )      \
    for( const char* enabled_extension : extensions ){                      \
        if(!strcmp( enabled_extension, extension ) ){                       \
            name = (PFN_##name)vkGetDeviceProcAddr( device, #name );        \
            DEBUG_CHECK_INV("Could not load instance-level Vulkan function named: "#name, name)\
        }\
    }
    #include "vulkan_function_list.inl"
    return 0;
}
