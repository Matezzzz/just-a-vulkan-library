#include "synchronization.h"
#include "../01_device/allocator.h"

Semaphore::Semaphore(){
    //                                                                  pNext    no flags
    VkSemaphoreCreateInfo info{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0};
    m_semaphore = g_allocator.get().createSemaphore(info);
}
const VkSemaphore* Semaphore::getPtr() const{
    return &m_semaphore;
}
Semaphore::operator VkSemaphore() const{
    return m_semaphore;
}


Fence::Fence() : m_fence(Fence::create())
{}
Fence::Fence(VkFence fence) : m_fence(fence)
{}
void Fence::reset(){
    //                                        fence count, 
    VkResult result = vkResetFences(g_device, 1, &m_fence);
    DEBUG_CHECK("Reset fence", result)
}
bool Fence::waitFor(long long unsigned int timeout) const{
    //wait_for_all - true->wait for all fences to be set / false->wait until one fence is set. Value doesn't matter for one fence
    //                                          fence count,,wait_for_all
    VkResult result = vkWaitForFences(g_device, 1, &m_fence, true, timeout);
    switch(result){
    case VK_SUCCESS:
        //return true if fence condition was true
        return true;
    case VK_TIMEOUT:
        //return false on timeout
        return false;
    default:
        //if command returned any error, print it's value and return false
        DEBUG_CHECK("Wait for fences", result);
        return false;
    }
}
const VkFence* Fence::getPtr() const{
    return &m_fence;
}
Fence::operator VkFence() const{
    return m_fence;
}
bool Fence::valid() const{
    return (m_fence != VK_NULL_HANDLE);
}
Fence Fence::create(VkFenceCreateFlags flags){
    //create fence with given flags and return it
    VkFenceCreateInfo info{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, flags};
    return Fence(g_allocator.get().createFence(info));
}



SignaledFence::SignaledFence(VkFenceCreateFlags flags) : Fence(Fence::create(flags))
{}



SubmitSynchronization::SubmitSynchronization() : m_end_fence(VK_NULL_HANDLE)
{}
SubmitSynchronization& SubmitSynchronization::setEndFence(VkFence fence){
    m_end_fence = Fence(fence);
    return *this;
}
bool SubmitSynchronization::hasEndFence() const{
    return m_end_fence.valid();
}
SubmitSynchronization& SubmitSynchronization::addEndSemaphore(VkSemaphore semaphore){
    m_end_semaphores.push_back(semaphore);
    return *this;
}
SubmitSynchronization& SubmitSynchronization::addStartSemaphore(VkSemaphore semaphore, VkPipelineStageFlags start_pipeline_flags){
    m_start_semaphores.push_back(semaphore);
    m_start_semaphores_stage_flags.push_back(start_pipeline_flags);
    return *this;
}
void SubmitSynchronization::waitFor(uint32_t timeout){
    //if there is no end fance to wait for, print error and return false
    if (!m_end_fence.valid()){
        PRINT_ERROR("No end fence set, nothing to wait for")
    }
    //otherwise, wait for the fence
    if (m_end_fence.waitFor(timeout)){
        m_end_fence.reset();
    }else{
        PRINT_ERROR("Waiting for queue expired")
    }
}
Fence SubmitSynchronization::getEndFence() const{
    return m_end_fence;
}
uint32_t SubmitSynchronization::getEndSemaphoreCount() const{
    return m_end_semaphores.size();
}
const VkSemaphore* SubmitSynchronization::getEndSemaphores() const{
    //if there are any end semaphores, return pointer to their array, otherwise return nullptr
    return (m_end_semaphores.size()) ? m_end_semaphores.data() : nullptr;
}
uint32_t SubmitSynchronization::getStartSemaphoreCount() const{
    return m_start_semaphores.size();
}
const VkSemaphore* SubmitSynchronization::getStartSemaphores() const{
    //if there are any start semaphores, return pointer to their array, otherwise return nullptr
    return (m_start_semaphores.size()) ? m_start_semaphores.data() : nullptr;
}
const VkPipelineStageFlags* SubmitSynchronization::getStartSemaphoresStageFlags() const{
    //if there are any semaphore stage flags, return pointer to their array, otherwise return nullptr
    return (m_start_semaphores_stage_flags.size()) ? m_start_semaphores_stage_flags.data() : nullptr;
}
VkSubmitInfo SubmitSynchronization::getSubmitInfo(const VkCommandBuffer* buf, uint32_t buf_count) const{
    //fill submit info structure and return it
    return VkSubmitInfo{
        VK_STRUCTURE_TYPE_SUBMIT_INFO, nullptr, 
        getStartSemaphoreCount(), getStartSemaphores(), getStartSemaphoresStageFlags(), 
        buf_count, buf, 
        getEndSemaphoreCount(), getEndSemaphores()
    };
}