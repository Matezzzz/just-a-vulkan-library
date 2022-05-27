#include "allocator.h"

AllocatorWrapper g_allocator;


//macro to define create functions for simple types
#define create(type, name)                  \
Vk##type VulkanAllocator::create##type(const Vk##type##CreateInfo & create_info){\
    Vk##type name;                          \
    VkResult result = vkCreate##type(m_device, &create_info, nullptr, &name);\
    DEBUG_CHECK("Create "#name, result)     \
    m_##name##s.push_back(name);            \
    return name;                            \
}

//macro to define for cycle to delete all objects
#define delete(type, name)\
for (Vk##type name : m_##name##s){              \
    vkDestroy##type(m_device, name, nullptr);   \
}



VulkanAllocator::VulkanAllocator(VkDevice device, VkPhysicalDevice physical_device) : 
    m_device(device), m_physical_device(physical_device)
{
    //get device properties, then save device limits from them
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);
    m_device_limits = properties.limits;

    //get device memory properties
    vkGetPhysicalDeviceMemoryProperties(physical_device, &m_memory_properties);
}

//define create functions for all simple types
functionForAllTypes(create)


VkDeviceMemory VulkanAllocator::allocateMemory(VkDeviceSize size, uint32_t type_bits, VkMemoryPropertyFlags properties){
    //Go over all memory types
    for (uint32_t i = 0; i < m_memory_properties.memoryTypeCount; i++){
        //if memory is of correct type and has correct properties
        if (((1 << i) & type_bits) && (m_memory_properties.memoryTypes[i].propertyFlags & properties) == properties){
            //define allocate info structure
            VkMemoryAllocateInfo allocate_info = {
                VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                nullptr, size, i};
            //allocate memory
            VkDeviceMemory memory;
            VkResult result = vkAllocateMemory(m_device, &allocate_info, nullptr, &memory);
            //add it to vector of allocated memory and return it
            m_allocated_memory.push_back(memory);
            DEBUG_CHECK("Memory allocation", result)
            return memory;
        }
    }
    //if no type of memory had the correct properties, print error
    DEBUG_ERROR("Suitable memory not found")
    return VK_NULL_HANDLE;
}
void* VulkanAllocator::mapMemory(VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size){
    //get pointer to the memory
    void* ptr;
    VkResult result = vkMapMemory(m_device, memory, offset, size, 0, &ptr);
    DEBUG_CHECK("Memory mapping", result)
    //if memory has been mapped before, return the pointer
    for (VkDeviceMemory m : m_mapped_memory){
        if (m == memory) return ptr;
    }
    //if it isn't mapped yet, add it to the vector of mapped memory and return pointer
    m_mapped_memory.push_back(memory);
    return ptr;
}
VkPipeline VulkanAllocator::createPipeline(const VkGraphicsPipelineCreateInfo& info){
    //Create a graphics pipeline using given info, add it to vector of pipelines, then return it's handle
    VkPipeline graphics_pipeline;
    VkResult result = vkCreateGraphicsPipelines(m_device, nullptr, 1, &info, nullptr, &graphics_pipeline);
    DEBUG_CHECK("Create graphics pipeline", result)
    m_pipelines.push_back(graphics_pipeline);
    return graphics_pipeline;
}
VkPipeline VulkanAllocator::createPipeline(const VkComputePipelineCreateInfo& info){
    //create a compute pipeline using given info, add it to vector of pipelines, then return it's handle
    VkPipeline compute_pipeline;
    VkResult result = vkCreateComputePipelines(m_device, nullptr, 1, &info, nullptr, &compute_pipeline);
    DEBUG_CHECK("Create compute pipeline", result)
    m_pipelines.push_back(compute_pipeline);
    return compute_pipeline;
}
VkSwapchainKHR VulkanAllocator::createSwapchain(const VkSwapchainCreateInfoKHR& info){
    //create a swapchain using given info, add it to vector of swapchains, then return it's handle
    VkSwapchainKHR swapchain;
    VkResult result = vkCreateSwapchainKHR(m_device, &info, nullptr, &swapchain);
    DEBUG_PRINT("Create swapchain", result)
    m_swapchains.push_back(swapchain);
    return swapchain;
}
const VkPhysicalDeviceLimits& VulkanAllocator::getLimits() const{
    return m_device_limits;
}
VkDevice VulkanAllocator::getDevice() const{
    return m_device;
}
VkPhysicalDevice VulkanAllocator::getPhysicalDevice() const{
    return m_physical_device;
}


void VulkanAllocator::destroy()
{
    //delete all elements in simple type vectors
    functionForAllTypes(delete)
    //unmap all mapped memory, then deallocate all memory
    for (VkDeviceMemory m : m_mapped_memory){
        vkUnmapMemory(m_device, m);
    }
    for (VkDeviceMemory m : m_allocated_memory){
        vkFreeMemory(m_device, m, nullptr);
    }
    
    //delete all pipelines and swapchains
    for (VkPipeline p : m_pipelines){
        vkDestroyPipeline(m_device, p, nullptr);
    }
    for (VkSwapchainKHR s : m_swapchains){
        vkDestroySwapchainKHR(m_device, s, nullptr);
    }
}



void AllocatorWrapper::set(VulkanAllocator& allocator)
{
    if (m_allocator == nullptr)
    {
        m_allocator = &allocator;
    }
    else
    {
        DEBUG_ERROR("Global allocator variable already set")
    }
}
AllocatorWrapper::operator VulkanAllocator&()
{
    return *m_allocator;
}
VulkanAllocator& AllocatorWrapper::get()
{
    return *m_allocator;
}

#undef create
#undef delete