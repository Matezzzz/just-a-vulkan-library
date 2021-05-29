#include "command_pool.h"

#include "../01_device/allocator.h"
#include "command_buffer.h"


CommandPoolInfo::CommandPoolInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags) :
    m_info{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO, nullptr, flags, queue_family_index}
{}
CommandPool CommandPoolInfo::create() const{
    return CommandPool(g_allocator.get().createCommandPool(m_info));
}



CommandPool::CommandPool(VkCommandPool command_pool) : m_command_pool(command_pool)
{}
void CommandPool::reset(bool release_resources){
    VkResult result = vkResetCommandPool(g_device, m_command_pool, release_resources ? VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT : 0);
    DEBUG_CHECK("Reset command pool", result)
}
void CommandPool::destroy(){
    vkDestroyCommandPool(g_device, m_command_pool, nullptr);
    DEBUG_CHECK("Destroy command pool", 0)
}

CommandBuffer CommandPool::allocateBuffer(VkCommandBufferLevel level){
    //                                                                                                                        how many to allocate
    VkCommandBufferAllocateInfo allocate_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, m_command_pool, level, 1};
    VkCommandBuffer buffer;
    VkResult result = vkAllocateCommandBuffers(g_device, &allocate_info, &buffer);
    DEBUG_CHECK("Allocate command buffer", result);
    //use handle to instantiate command buffer object, then return it
    return CommandBuffer(buffer);
}

vector<CommandBuffer> CommandPool::allocateBuffers(uint32_t buffer_count, VkCommandBufferLevel level){
    VkCommandBufferAllocateInfo allocate_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, nullptr, m_command_pool, level, buffer_count};
    vector<VkCommandBuffer> buffs(buffer_count);
    VkResult result = vkAllocateCommandBuffers(g_device, &allocate_info, &buffs[0]);
    DEBUG_CHECK("Allocate command buffers", result);
    
    //convert each VkCommandBuffer to CommandBuffer class
    vector<CommandBuffer> buffers;
    for (VkCommandBuffer b : buffs){
        buffers.push_back(CommandBuffer(b));
    }
    return buffers;
}




