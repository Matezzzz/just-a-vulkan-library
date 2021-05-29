#ifndef VULKAN_FUNCTIONS_H
#define VULKAN_FUNCTIONS_H

/**
 * vulkan_base.h
 *  - Holds forward declarations for all vulkan functions
 *  - Holds debug printing functions
 */


#define VK_NO_PROTOTYPES
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#else
#error Currently, compilation works only on Windows because of how the library is loaded. Theoretically, all classes and functions except class VulkanLibrary and func loadVulkanFunctions() should be platform-agnostic. It should be relatively easy to convert both functions to their linux/mac versions if needed.
#endif 

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>


using std::vector;
using std::string;


//The version of vulkan to use
constexpr uint32_t VULKAN_VERSION = VK_MAKE_VERSION(1, 2, 154);


//Definitions for all vulkan functions
#define EXPORTED_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION( name ) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION( name, extension ) extern PFN_##name name;

//include list of functions to forward declare all of them
#include "vulkan_function_list.inl"

//Load all global vulkan functions (Set their previously declared pointers)
int loadVulkanFunctions(HMODULE& vulkan_library);
//Load all instance level vulkan functions (Set their previously declared pointers)
int loadInstanceLevelFunctions(VkInstance& instance, const vector<const char*>& extensions);
//Load all device level functions (Set their previously declared pointers)
int loadDeviceLevelFunctions(VkDevice& device, const vector<const char*>& extensions);

//handle to windows console to enable colored output
extern HANDLE hConsole;

//where to output all debug results
#define DEBUG_OUT std::cout
//print green text
#define PRINT_SUCCESS(text) {SetConsoleTextAttribute(hConsole, 2); DEBUG_OUT << text << ".\n"; SetConsoleTextAttribute(hConsole, 15);}
//print yellow text
#define PRINT_WARN(text) {SetConsoleTextAttribute(hConsole, 14); DEBUG_OUT << text << ".\n"; SetConsoleTextAttribute(hConsole, 15);}
//print cyan? text with file and line
#define PRINT_DEBUG(text) {SetConsoleTextAttribute(hConsole, 11); DEBUG_OUT << __FILE__ << ":" << __LINE__ << " : " << text << ".\n"; SetConsoleTextAttribute(hConsole, 15);}
//print red text with relative path to file and line where error happened
#define PRINT_ERROR(text) {SetConsoleTextAttribute(hConsole, 4); DEBUG_OUT << __FILE__ << ":" << __LINE__ << " : " << text << ".\n"; SetConsoleTextAttribute(hConsole, 15);}
//if (result != 0), print error with name and location of occurence
#define DEBUG_CHECK(name, result) if (result) {PRINT_ERROR(name << " failed. Code: " << result); throw std::runtime_error(name);}
//if (result == 0), print error with name and location of occurence
#define DEBUG_CHECK_INV(name, result) if (!result) {PRINT_ERROR(name << " failed."); throw std::runtime_error(name);}


//define to enable more thorough information about what is currently happening
#define DEBUG

//if debug, success of the major operations is printed too
#ifdef DEBUG
#define DEBUG_PRINT(name, result) if (result) {PRINT_ERROR(name << " failed. Code: " << result); throw std::runtime_error(name);} else {PRINT_SUCCESS(name << " : OK")}
#define DEBUG_PRINT_INV(name, result) if (!result) {PRINT_ERROR(name << " failed."); throw std::runtime_error(name);} else {PRINT_SUCCESS(name << " : OK")}
//otherwise, print only errors
#else
#define DEBUG_PRINT(name, result) DEBUG_CHECK(name, result)
#define DEBUG_PRINT_INV(name, result) DEBUG_CHECK_INV(name, result)
#endif

#endif // VULKAN_FUNCTIONS_H
