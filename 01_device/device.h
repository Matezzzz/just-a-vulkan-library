#ifndef DEVICE_H
#define DEVICE_H

/**
 * device.h
 *  - classes for Device(vulkan logical device) and Queue 
 */

#include "../00_base/vulkan_base.h"
#include "vulkan_instance.h"
#include "allocator.h"

class Queue;
class SubmitSynchronization;


/**
 * Device
 *  - Represents one vulkan (logical) device
 */
class Device{
    VkDevice m_device;
    //The allocator to hold all objects created on this device
    VulkanAllocator m_allocator;
    //All queues - one vector<Queue> for each VkDeviceQueueCreateInfo passed to constructor
    vector<vector<Queue>> m_queues;
public:
    /**
     * Set device handle to the one passed in, then get all reserved queues
     * @param device
     * @param physical_device
     * @param queue_infos the queue infos, with which the device was created, now used to request queues from created device
     */
    Device(VkDevice device, VkPhysicalDevice physical_device, const vector<VkDeviceQueueCreateInfo>& queue_infos);

    //Wait for all operations on the device to finish executing 
    void waitFor() const;
    //Destroy the device and all objects on it
    void destroy();

    const VkDevice& get() const;
    operator VkDevice();
    //Get a reference to created queue
    Queue& getQueue(int group, int index);
};


/**
 * Queue
 *  - One vulkan Queue
 */
class Queue
{
    VkQueue m_queue;
    //Index of the family to which the queue belongs
    uint32_t m_family_index;
public:
    Queue(VkQueue queue, uint32_t family_index);
    //Wait for everything on the queue to finish executing
    void waitFor() const;
    /**
     * Submit the given command buffer to this queue
     * @param command_buffer
     * @param submit_synchronization holds all synchronization elements to check whether execution has started/ended (fences/semaphores)
     */
    void submit(VkCommandBuffer command_buffer, const SubmitSynchronization& submit_synchronization);
    //Return family index of the queue
    uint32_t getFamilyIndex() const;
    operator VkQueue() const;
};

#endif