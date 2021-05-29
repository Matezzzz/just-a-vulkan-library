#include "extension_utilities.h"

void AvailableExtensionList::extensionsAvailable(const vector<string>& desired_extensions, vector<string>& resulting_extensions) const{
    //if there was a query for available extensions without actually requesting any
    if (desired_extensions.size() == 0) PRINT_WARN("No extensions requested.")
    
    // Iterate over all desired extensions and check which ones are available
    for (const string& extension : desired_extensions){
        bool found = false;
        //iterate over all avaliable extensions
        for (const VkExtensionProperties& prop : m_available_extensions){
            //if names are the same, extension was found
            if (!strcmp(prop.extensionName, extension.c_str())){
                found = true;
                break;
            }
        }
        //if extension has been found, add it to the vector of extensions that can be used, otherwise, print error
        if (found){
            resulting_extensions.push_back(extension);
        }else{
            PRINT_ERROR("Extension not available: " << extension)
        }
    }
}

//Load instance level extensions into m_available_extensions list
InstanceExtensionList::InstanceExtensionList(){
    uint32_t extension_count = 0;
    VkResult result = VK_SUCCESS;
    //get instance extension count
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
    DEBUG_CHECK("Could not get instance extension count", result)
    
    //reserve space for extensions
    m_available_extensions.resize(extension_count);
    //load instance extension properties into vector
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, &m_available_extensions[0]);
    DEBUG_CHECK("Could not get instance extension properties", result)
}

DeviceExtensionList::DeviceExtensionList(const VkPhysicalDevice& device)
{
    uint32_t extension_count = 0;
    VkResult result = VK_SUCCESS;
    //get device extension count
    result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    DEBUG_CHECK("Could not get device extension count", result)

    //reserve space for extensions
    m_available_extensions.resize(extension_count);
    //load device level extension properties into vector
    result = vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, &m_available_extensions[0]);
    DEBUG_CHECK("Could not get device extension properties", result)
} 

 


vector<const char*> convertToVectorOfPointers(const vector<string>& vec){  
    //allocate vector for the pointers
    vector<const char*> result(vec.size());
    //go through strings and set pointers to the correct values
    for (uint32_t i = 0; i < vec.size(); i++){
        result[i] = vec[i].c_str();
    }
    return result;
}
