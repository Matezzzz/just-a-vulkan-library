#include "physical_device.h"


//each function sets one flag to true in m_features struct
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableRobustBufferAccess(){m_features.robustBufferAccess = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableFullDrawIndexUint32(){m_features.fullDrawIndexUint32 = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableImageCubeArray(){m_features.imageCubeArray = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableIndependentBlend(){m_features.independentBlend = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableGeometryShader(){m_features.geometryShader = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableTessellationShader(){m_features.tessellationShader = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSampleRateShading(){m_features.sampleRateShading = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableDualSrcBlend(){m_features.dualSrcBlend = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableLogicOp(){m_features.logicOp = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableMultiDrawIndirect(){m_features.multiDrawIndirect = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableDrawIndirectFirstInstance(){m_features.drawIndirectFirstInstance = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableDepthClamp(){m_features.depthClamp = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableDepthBiasClamp(){m_features.depthBiasClamp = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableFillModeNonSolid(){m_features.fillModeNonSolid = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableDepthBounds(){m_features.depthBounds = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableWideLines(){m_features.wideLines = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableLargePoints(){m_features.largePoints = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableAlphaToOne(){m_features.alphaToOne = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableMultiViewport(){m_features.multiViewport = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSamplerAnisotropy(){m_features.samplerAnisotropy = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableTextureCompressionETC2(){m_features.textureCompressionETC2 = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableTextureCompressionASTC_LDR(){m_features.textureCompressionASTC_LDR = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableTextureCompressionBC(){m_features.textureCompressionBC = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableOcclusionQueryPrecise(){m_features.occlusionQueryPrecise = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enablePipelineStatisticsQuery(){m_features.pipelineStatisticsQuery = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableVertexPipelineStoresAndAtomics(){m_features.vertexPipelineStoresAndAtomics = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableFragmentStoresAndAtomics(){m_features.fragmentStoresAndAtomics = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderTessellationAndGeometryPointSize(){m_features.shaderTessellationAndGeometryPointSize = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderImageGatherExtended(){m_features.shaderImageGatherExtended = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageImageExtendedFormats(){m_features.shaderStorageImageExtendedFormats = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageImageMultisample(){m_features.shaderStorageImageMultisample = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageImageReadWithoutFormat(){m_features.shaderStorageImageReadWithoutFormat = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageImageWriteWithoutFormat(){m_features.shaderStorageImageWriteWithoutFormat = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderUniformBufferArrayDynamicIndexing(){m_features.shaderUniformBufferArrayDynamicIndexing = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderSampledImageArrayDynamicIndexing(){m_features.shaderSampledImageArrayDynamicIndexing = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageBufferArrayDynamicIndexing(){m_features.shaderStorageBufferArrayDynamicIndexing = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderStorageImageArrayDynamicIndexing(){m_features.shaderStorageImageArrayDynamicIndexing = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderClipDistance(){m_features.shaderClipDistance = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderCullDistance(){m_features.shaderCullDistance = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderFloat64(){m_features.shaderFloat64 = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderInt64(){m_features.shaderInt64 = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderInt16(){m_features.shaderInt16 = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderResourceResidency(){m_features.shaderResourceResidency = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableShaderResourceMinLod(){m_features.shaderResourceMinLod = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseBinding(){m_features.sparseBinding = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidencyBuffer(){m_features.sparseResidencyBuffer = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidencyImage2D(){m_features.sparseResidencyImage2D = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidencyImage3D(){m_features.sparseResidencyImage3D = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidency2Samples(){m_features.sparseResidency2Samples = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidency4Samples(){m_features.sparseResidency4Samples = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidency8Samples(){m_features.sparseResidency8Samples = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidency16Samples(){m_features.sparseResidency16Samples = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableSparseResidencyAliased(){m_features.sparseResidencyAliased = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableVariableMultisampleRate(){m_features.variableMultisampleRate = true; return *this;}
PhysicalDeviceFeatures& PhysicalDeviceFeatures::enableInheritedQueries(){m_features.inheritedQueries = true; return *this;}
const VkPhysicalDeviceFeatures& PhysicalDeviceFeatures::getFeatures() {return m_features;}

string feature_names[]{"RobustBufferAccess", "FullDrawIndexUint32", "ImageCubeArray", "IndependentBlend", "GeometryShader", "TessellationShader", "SampleRateShading", "DualSrcBlend", "LogicOp",
    "MultiDrawIndirect", "DrawIndirectFirstInstance", "DepthClamp", "DepthBiasClamp", "FillModeNonSolid", "DepthBounds", "WideLines", "LargePoints", "AlphaToOne", "MultiViewport", "SamplerAnisotropy",
    "TextureCompressionETC2", "TextureCompressionASTC_LDR", "TextureCompressionBC", "OcclusionQueryPrecise", "PipelineStatisticsQuery", "VertexPipelineStoresAndAtomics", "FragmentStoresAndAtomics", 
    "ShaderTessellationAndGeometryPointSize", "ShaderImageGatherExtended", "ShaderStorageImageExtendedFormats", "ShaderStorageImageMultisample", "ShaderStorageImageReadWithoutFormat",
    "ShaderStorageImageWriteWithoutFormat", "ShaderUniformBufferArrayDynamicIndexing", "ShaderSampledImageArrayDynamicIndexing", "ShaderStorageBufferArrayDynamicIndexing",
    "ShaderStorageImageArrayDynamicIndexing", "ShaderClipDistance", "ShaderCullDistance", "ShaderFloat64", "ShaderInt64", "ShaderInt16", "ShaderResourceResidency", "ShaderResourceMinLod", "SparseBinding",
    "SparseResidencyBuffer", "SparseResidencyImage2D", "SparseResidencyImage3D", "SparseResidency2Samples", "SparseResidency4Samples", "SparseResidency8Samples", "SparseResidency16Samples",
    "SparseResidencyAliased", "VariableMultisampleRate", "InheritedQueries"};


PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) : m_device(device)
{}
PhysicalDevice& PhysicalDevice::requestExtensions(const vector<string> desired_extensions){
    //Get available extensions
    DeviceExtensionList extensions(m_device);
    //Copy all extensions that are both desired and available to m_enabled_extensions
    extensions.extensionsAvailable(desired_extensions, m_enabled_extensions);
    return *this;
}
PhysicalDevice& PhysicalDevice::requestFeatures(const PhysicalDeviceFeatures& features){
    //get features supported on device
    VkPhysicalDeviceFeatures available_features;
    vkGetPhysicalDeviceFeatures(m_device, &available_features);

    //get pointers to starts and desired features. VkPhysicalDeviceFeatures is a struct filled only with VkBool32 variables, this makes it possible to iterate through them by casting struct to VkBOol32*
    const VkBool32* available_features_prt = reinterpret_cast<const VkBool32*>(&available_features);
    const VkBool32* desired_features_prt = reinterpret_cast<const VkBool32*>(&features);
    VkBool32* enabled_features_prt = reinterpret_cast<VkBool32*>(&m_enabled_features);
    //go through all the features
    for (uint32_t i = 0; i < sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); i++){
        //if user requested this feature
        if (desired_features_prt[i]){
            //if it's available, enable it, else print error
            if (available_features_prt[i]){
                enabled_features_prt[i] = desired_features_prt[i];
            }else{
                DEBUG_ERROR("Requested feature " << feature_names[i] << " is not available on this device")
            }
        }
    }
    return *this;
}
PhysicalDevice& PhysicalDevice::requestQueues(const vector<QueueRequestInfo>& queues, const vector<VkBool32>& m_usable_families){
    //get queue family count
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queue_family_count, nullptr);
    DEBUG_CHECK_INV("Couldn't get queue family count", queue_family_count)

    //read queue family properties
    vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queue_family_count, queue_family_properties.data());
    DEBUG_CHECK_INV("Couldn't get queue family properties", queue_family_count)
    
    //go through all requested queues
    for (const QueueRequestInfo& request : queues){
        bool found = false;
        //go through all queue families
        for (uint32_t queue_family_index = 0; queue_family_index < queue_family_properties.size(); queue_family_index++){
            //is m_usable_families.size() != 0, it is implied that some queue families are unusable due to factors not taken into account by this function
            //if current queue family is one of unusable ones, skip it and try next one
            if (m_usable_families.size() != 0 && !m_usable_families[queue_family_index]) continue;
            //if queues from a given family have requested properties
            if (queue_family_properties[queue_family_index].queueFlags & request.usage){
                //if there are enough queues in a given family
                if (queue_family_properties[queue_family_index].queueCount >= request.count){
                    //this request can be satisfied, compatible queues found
                    found = true;
                    //queue priorities specify how important the work in the given queue is, by default, these are all specified as 0.5f
                    m_queue_priorities.push_back(vector<float>(request.count, 0.5f));
                    //fill queue create info structure
                    VkDeviceQueueCreateInfo info{
                        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0,
                        queue_family_index, request.count, m_queue_priorities.back().data()
                    };
                    //subtract currently reserved queue count from the family it was taken from
                    queue_family_properties[queue_family_index].queueCount -= request.count;
                    //save queue create info
                    m_queue_infos.push_back(info);
                    break;
                }
            }
        }
        //if queue of given properties isn't available, print error
        if (!found){
            DEBUG_ERROR("Requested queues are not available")
        }
    }
    return *this;
}
PhysicalDevice& PhysicalDevice::requestScreenSupportQueues(const vector<QueueRequestInfo>& queues, Window& window){
    //find out how many queue families there are
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_device, &queue_family_count, nullptr);
    DEBUG_CHECK_INV("Couldn't get queue family count", queue_family_count)
    //find out which queue families support image presentation to the given window
    vector<VkBool32> usable_queue_families(queue_family_count);
    for (uint32_t queue_family_index = 0; queue_family_index < queue_family_count; queue_family_index++){
        vkGetPhysicalDeviceSurfaceSupportKHR(m_device, queue_family_index, window.getSurface(), &usable_queue_families[queue_family_index]);
    }
    //call request queues function with some queue families disabled
    return requestQueues(queues, usable_queue_families);
}
Device& PhysicalDevice::createLogicalDevice(VulkanInstance& instance){
    //convert internal vector of strings to a vector of const char*
    vector<const char*> extensions = convertToVectorOfPointers(m_enabled_extensions);
    
    //fill device create info
    VkDeviceCreateInfo device_create_info {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            nullptr, 0, (uint32_t) m_queue_infos.size(), m_queue_infos.data(),\
            0, nullptr, (uint32_t) extensions.size(), &extensions[0], &m_enabled_features};
    
    //call instance function to register created device 
    return instance.createLogicalDevice(device_create_info, m_device, m_queue_infos, extensions);
}
PhysicalDevice::operator VkPhysicalDevice() const{
    return m_device;
}



PhysicalDevices::PhysicalDevices(VkInstance instance){
    //get physical device count
    uint32_t device_count;
    VkResult result = vkEnumeratePhysicalDevices(instance, &device_count, nullptr);
    DEBUG_CHECK("Get physical device count", result)
    //allocate space for physical devices, then get their handles
    m_devices.resize(device_count);
    result = vkEnumeratePhysicalDevices(instance, &device_count, m_devices.data());
    DEBUG_CHECK("Get physical devices", result)
}
PhysicalDevice PhysicalDevices::choose(){
    VkPhysicalDeviceProperties properties;
    //if there are no devices supporting vulkan on the system, print error
    if (m_devices.size() == 0){
        DEBUG_ERROR("No vulkan compatible devices found")
        throw std::runtime_error("No vulkan devices found.");
    }
    //if there is one vulkan device, pick it
    else if (m_devices.size() == 1){
        vkGetPhysicalDeviceProperties(m_devices[0], &properties);
        PRINT_WARN("One vulkan device found: " << properties.deviceName)
        return PhysicalDevice(m_devices[0]);
    }else{
        //if there are multiple devices, print their names and let the user choose
        PRINT_WARN(m_devices.size() << " Vulkan compatible devices found")
        for (uint32_t i = 0; i < m_devices.size(); i++){
            vkGetPhysicalDeviceProperties(m_devices[i], &properties);
            PRINT_WARN(" " << i << ": " << properties.deviceName)
        }
        PRINT_WARN("Enter index of the device you want to use : ")
        //read index of device to use, then select given device
        uint32_t index;
        cin >> index;
        return PhysicalDevice(m_devices[index]);
    }
}