#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "../00_base/vulkan_base.h"



using nanoseconds = uint64_t; 

//second in nanoseconds
const nanoseconds SYNC_SECOND = 1000000000;
//a second
const nanoseconds A_SHORT_WHILE = SYNC_SECOND;
//frame in nanoseconds at 60FPS
const nanoseconds SYNC_FRAME = 16666666;
//frame time in nanoseconds at 144FPS
const nanoseconds SYNC_FRAME144 = 6944444;

const nanoseconds SYNC_1MS = 1000000;
const nanoseconds SYNC_10US = 10000;


/**
 * Semaphore
 *  - is used for synchronization purely on the GPU side
 */
class Semaphore{
protected:
    VkSemaphore m_semaphore;
public:
    Semaphore();
    const VkSemaphore* getPtr() const;
    operator VkSemaphore() const;
};





/**
 * Fence
 *  - is used for synchronization between CPU and GPU
 */
class Fence{
    VkFence m_fence;
public:
    //create new unsignaled fence
    Fence();

    //create fence object from given handle
    Fence(VkFence fence);

    //reset fence to its' default, unsignaled state
    void reset();
    
    /**
     * Wait for the fence to become signaled, return true if it did inside given time, false if time ran out first.
     * @param timeout time in nanoseconds
     */
    bool waitFor(nanoseconds timeout) const;
    const VkFence* getPtr() const;
    operator VkFence() const;

    //return (m_fence != VK_NULL_HANDLE);
    bool valid() const;
protected:
    //Create new fence, flags can be 0 for unsignaled VK_FENCE_CREATE_SIGNALED_BIT to create a signaled fence    
    static Fence create(VkFenceCreateFlags flags = 0);
};


/**
 * SignaledFence
 *  - A fence, whose constructor accepts flags. This can make the fence signaled when initialized
 *  - Default class Fence doesn't have this constructor due to type collision (VkFenceCreateFlags and VkFence are both integers)
 */
class SignaledFence : public Fence{
public:
    //Create a fence with given flags. Is signaled by default.
    SignaledFence(VkFenceCreateFlags flags = VK_FENCE_CREATE_SIGNALED_BIT);
};





class SubmitSynchronization{
    vector<VkSemaphore> m_start_semaphores;
    vector<VkPipelineStageFlags> m_start_semaphores_stage_flags;

    Fence m_end_fence;
    vector<VkSemaphore> m_end_semaphores;
public:
    /**
     * Create a new submit synchronization without any fences or semaphores.
     */
    SubmitSynchronization();
    
    /**
     * Set a fence that will be signaled when command buffer submit ends.
     * @param fence the fence to set
     */
    SubmitSynchronization& setEndFence(VkFence fence);

    /**
     * Create and set a fence that will be signaled when command buffer submit ends.
     */
    SubmitSynchronization& addEndFence();

    /**
     * Returns true if there was an end fence set.
     */
    bool hasEndFence() const;
    
    /**
     * Add a semaphore that will be signaled when command buffer submit ends.
     * @param semaphore the semaphore to add
     */
    SubmitSynchronization& addEndSemaphore(VkSemaphore semaphore);

    /**
     * Add a semaphore to wait for before starting execution of a given pipeline stage.
     * @param semaphore the semaphore to add
     * @param start_pipeline_flags the pipeline stage to wait at
     */
    SubmitSynchronization& addStartSemaphore(VkSemaphore semaphore, VkPipelineStageFlags start_pipeline_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

    /**
     * Check whether all command buffers have finished - the fence is signalled.
     * @param timeout how long to wait before returning false
     */
    void waitFor(nanoseconds timeout);

    /**
     * Returns fence if set, else VK_NULL_HANDLE.
     */
    Fence getEndFence() const;

    /**
     * Get end semaphore count.
     */
    uint32_t getEndSemaphoreCount() const;

    /**
     * Get a pointer to the start of end semaphores array, or nullptr if there are none.
     */
    const VkSemaphore* getEndSemaphores() const;


    /**
     * Get start semaphore count.
     */
    uint32_t getStartSemaphoreCount() const;

    /**
     * Get a pointer to the start of start semaphores array, or nullptr if there are none.
     */
    const VkSemaphore* getStartSemaphores() const;

    /**
     * Get a pointer to the start of start semaphore stage flags array, or nullptr if there are none.
     */
    const VkPipelineStageFlags* getStartSemaphoresStageFlags() const;

    /**
     * Return a command buffer submit info struct using all semaphores from this submit info.
     * @param buf pointer to all command buffers to submit to queue
     * @param buf_count - how many command buffers to submit
     */
    VkSubmitInfo getSubmitInfo(const VkCommandBuffer* buf, uint32_t buf_count = 1) const;
};

#endif
