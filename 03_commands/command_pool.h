#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include "../00_base/vulkan_base.h"

class CommandBuffer;



/**
 * CommandPool
 *  - Holds vulkan command pool - a object from which command buffers can be allocated
 */
class CommandPool{
    VkCommandPool m_command_pool;
public:
    CommandPool(VkCommandPool command_pool);

    /**
     * Reset the command pool and turn all allocated buffers into state before recording.
     * @param release_resources true to release, false to keep
     */
    void reset(bool release_resources);

    //destroy this command pool and all child command buffers
    void destroy();

    /**
     * Allocate one command buffer
     * @param level can be either VK_COMMAND_BUFFER_LEVEL_PRIMARY or VK_COMMAND_BUFFER_LEVEL_SECONDARY - primary buffers are submitted to queues, secondary ones are embedded in primary ones
     */
    CommandBuffer allocateBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    /**
     * Allocate multiple buffers of same level at once
     * @param level can be either VK_COMMAND_BUFFER_LEVEL_PRIMARY or VK_COMMAND_BUFFER_LEVEL_SECONDARY - primary buffers are submitted to queues, secondary ones are embedded in primary ones
     */
    vector<CommandBuffer> allocateBuffers(uint32_t buffer_count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
};


/**
 * CommandPoolInfo
 *  - Information needed for creating command buffer pool
 */
class CommandPoolInfo
{
    VkCommandPoolCreateInfo m_info;
public:
    /**
     * @param queue_family_index only queues from this family will be able to use command buffers from this pool
     * @param flags either 0 or combination of VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT and VK_COMMAND_POOL_CREATE_PROTECTED_BIT
     */
    CommandPoolInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags = 0);

    /**
     * Create the command pool
     */
    CommandPool create() const;
};

#endif