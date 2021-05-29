#ifndef PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

/**
 * physical_device.h
 *  - Holds all classes related to picking a physical device out of the ones available and creating logical device on it
 */


#include "../00_base/vulkan_base.h"
#include "../00_base/extension_utilities.h"
#include "../02_swapchain/window.h"
#include <iostream>

using std::cout;
using std::cin;

#include "vulkan_instance.h"
#include "device.h"



/**
 * PhysicalDeviceFeatures
 *  - Makes it possible to quickly specify which device features the user requests
 *  - Check https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceFeatures.html for more information about what each flag does
 */
class PhysicalDeviceFeatures{
    VkPhysicalDeviceFeatures m_features;
public:
    PhysicalDeviceFeatures& enableRobustBufferAccess();
    PhysicalDeviceFeatures& enableFullDrawIndexUint32();
    PhysicalDeviceFeatures& enableImageCubeArray();
    PhysicalDeviceFeatures& enableIndependentBlend();
    PhysicalDeviceFeatures& enableGeometryShader();
    PhysicalDeviceFeatures& enableTessellationShader();
    PhysicalDeviceFeatures& enableSampleRateShading();
    PhysicalDeviceFeatures& enableDualSrcBlend();
    PhysicalDeviceFeatures& enableLogicOp();
    PhysicalDeviceFeatures& enableMultiDrawIndirect();
    PhysicalDeviceFeatures& enableDrawIndirectFirstInstance();
    PhysicalDeviceFeatures& enableDepthClamp();
    PhysicalDeviceFeatures& enableDepthBiasClamp();
    PhysicalDeviceFeatures& enableFillModeNonSolid();
    PhysicalDeviceFeatures& enableDepthBounds();
    PhysicalDeviceFeatures& enableWideLines();
    PhysicalDeviceFeatures& enableLargePoints();
    PhysicalDeviceFeatures& enableAlphaToOne();
    PhysicalDeviceFeatures& enableMultiViewport();
    PhysicalDeviceFeatures& enableSamplerAnisotropy();
    PhysicalDeviceFeatures& enableTextureCompressionETC2();
    PhysicalDeviceFeatures& enableTextureCompressionASTC_LDR();
    PhysicalDeviceFeatures& enableTextureCompressionBC();
    PhysicalDeviceFeatures& enableOcclusionQueryPrecise();
    PhysicalDeviceFeatures& enablePipelineStatisticsQuery();
    PhysicalDeviceFeatures& enableVertexPipelineStoresAndAtomics();
    PhysicalDeviceFeatures& enableFragmentStoresAndAtomics();
    PhysicalDeviceFeatures& enableShaderTessellationAndGeometryPointSize();
    PhysicalDeviceFeatures& enableShaderImageGatherExtended();
    PhysicalDeviceFeatures& enableShaderStorageImageExtendedFormats();
    PhysicalDeviceFeatures& enableShaderStorageImageMultisample();
    PhysicalDeviceFeatures& enableShaderStorageImageReadWithoutFormat();
    PhysicalDeviceFeatures& enableShaderStorageImageWriteWithoutFormat();
    PhysicalDeviceFeatures& enableShaderUniformBufferArrayDynamicIndexing();
    PhysicalDeviceFeatures& enableShaderSampledImageArrayDynamicIndexing();
    PhysicalDeviceFeatures& enableShaderStorageBufferArrayDynamicIndexing();
    PhysicalDeviceFeatures& enableShaderStorageImageArrayDynamicIndexing();
    PhysicalDeviceFeatures& enableShaderClipDistance();
    PhysicalDeviceFeatures& enableShaderCullDistance();
    PhysicalDeviceFeatures& enableShaderFloat64();
    PhysicalDeviceFeatures& enableShaderInt64();
    PhysicalDeviceFeatures& enableShaderInt16();
    PhysicalDeviceFeatures& enableShaderResourceResidency();
    PhysicalDeviceFeatures& enableShaderResourceMinLod();
    PhysicalDeviceFeatures& enableSparseBinding();
    PhysicalDeviceFeatures& enableSparseResidencyBuffer();
    PhysicalDeviceFeatures& enableSparseResidencyImage2D();
    PhysicalDeviceFeatures& enableSparseResidencyImage3D();
    PhysicalDeviceFeatures& enableSparseResidency2Samples();
    PhysicalDeviceFeatures& enableSparseResidency4Samples();
    PhysicalDeviceFeatures& enableSparseResidency8Samples();
    PhysicalDeviceFeatures& enableSparseResidency16Samples();
    PhysicalDeviceFeatures& enableSparseResidencyAliased();
    PhysicalDeviceFeatures& enableVariableMultisampleRate();
    PhysicalDeviceFeatures& enableInheritedQueries();

    //get a reference to features struct
    const VkPhysicalDeviceFeatures& getFeatures();
};


//Struct for holding infos when requesting a queue
struct QueueRequestInfo{
    uint32_t count;
    VkQueueFlags usage;
};

/**
 * PhysicalDevice
 *  - All information about one physical device, and the data needed to create logical device out of it
 */
class PhysicalDevice{
    VkPhysicalDevice m_device;

    //device level extensions enabled
    vector<string> m_enabled_extensions{};
    VkPhysicalDeviceFeatures m_enabled_features{};

    //queue priorities, one float per queue to allocate
    vector<vector<float>> m_queue_priorities;
    //queue creation infos
    vector<VkDeviceQueueCreateInfo> m_queue_infos;
public:
    PhysicalDevice(VkPhysicalDevice device);
    
    /**
     * Request given queues on the device
     * @param queues the information about queues being requested
     * @param m_usable_families does nothing if left empty, can be used to ignore some queue families, is used by the following function requestScreenSupportQueues()
     */
    PhysicalDevice& requestQueues(const vector<QueueRequestInfo>& queues, const vector<VkBool32>& m_usable_families = {});
    
    /**
     * Request queues with image presentation support and given parameters on the device
     * @param queues the information about queues being requested
     * @param window the window on which the queues should be able to present
     */
    PhysicalDevice& requestScreenSupportQueues(const vector<QueueRequestInfo>& queues, Window& window);

    //Request extensions with given names
    PhysicalDevice& requestExtensions(const vector<string> extensions);

    //Request given features
    PhysicalDevice& requestFeatures(const PhysicalDeviceFeatures& features);

    //Create logical device with parameters given by the request functions
    Device& createLogicalDevice(VulkanInstance& instance);
    operator VkPhysicalDevice() const;
};

/**
 * PhysicalDevices
 *  - List of all physical devices
 */
class PhysicalDevices
{
    vector<VkPhysicalDevice> m_devices;
public:
    PhysicalDevices(VkInstance instance);
    
    /**
     * Let the user choose the vulkan compatible device to use 
     *  - if there are none, print error
     *  - if there is one, pick it
     *  - if there are more, let the user choose by typing a number into std::cin
     */ 
    PhysicalDevice choose();
};

#endif