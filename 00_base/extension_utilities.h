#ifndef EXTENSION_UTILITIES_H
#define EXTENSION_UTILITIES_H


/**
 * extension_utilities.h
 *  - Classes related to checking whether requested vulkan extensions are available.
 */

#include "vulkan_base.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

//AvailableExtensionList - Base class for list of extensions
class AvailableExtensionList{
protected:
    //VkExtensionProperties structure holds extension version and name
    vector<VkExtensionProperties> m_available_extensions;
public:
    //Check whether desired extensions are available, save those that are into resulting extensions
    void extensionsAvailable(const vector<string>& desired_extensions, vector<string>& resulting_extensions) const;
};


//InstanceExtensionList - Holds all available instance level extensions 
class InstanceExtensionList : public AvailableExtensionList{
public:
    //Load all instance level extensions into m_available_extensions vector
    InstanceExtensionList();
};


//DeviceExtensionList - Holds all available device level extensions for given physical device
class DeviceExtensionList : public AvailableExtensionList{
public:
    //Load all device level extensions into m_available_extensions vector
    DeviceExtensionList(const VkPhysicalDevice& device);
};


//return a vector of pointers to first characters of strings
vector<const char*> convertToVectorOfPointers(const vector<string>& vec);


#endif // EXTENSION_UTILITIES_H
