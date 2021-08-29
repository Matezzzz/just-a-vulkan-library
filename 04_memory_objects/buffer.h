#ifndef BUFFER_H
#define BUFFER_H

#include "../00_base/vulkan_base.h"
#include "mixed_buffer.h"


enum BufferStatesEnum{
    BUFFER_INVALID,
    BUFFER_NEWLY_CREATED,
    BUFFER_UNIFORM,
    BUFFER_STORAGE_R,
    BUFFER_STORAGE_W,
    BUFFER_STORAGE_RW
};
constexpr VkAccessFlags buffer_states_accesses[]{
    0,
    0,
    VK_ACCESS_SHADER_READ_BIT,
    VK_ACCESS_SHADER_READ_BIT,
    VK_ACCESS_SHADER_WRITE_BIT,
    VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
};


class BufferState{
public:
    VkAccessFlags access;

    BufferState() : access(0)
    {}

    BufferState(BufferStatesEnum state) : access(buffer_states_accesses[state])
    {}
    /*BufferState(VkAccessFlags access_) : access(access_)
    {}*/
};



/**
 * Buffer
 *  - manages one VkBuffer
 */
class Buffer{  
    VkBuffer m_buffer;
    //size in bytes
    VkDeviceSize m_size;
public:
    //construct invalid buffer
    Buffer();
    //construct buffer. Info is passed in to set m_size
    Buffer(VkBuffer buffer, const VkBufferCreateInfo& info);
    /**
     * Create memory barrier for given buffer.
     * @param current_access how was the buffer used until now
     * @param new_access how will the buffer be used
     * @param current_queue_family the queue family that has used the buffer until now, can be ignored if it isn't supposed to change
     * @param new_queue_family the queue family that will use the buffer from now, can be ignored if it isn't supposed to change
     */
    VkBufferMemoryBarrier createMemoryBarrier(VkAccessFlags current_access, VkAccessFlags new_access, uint32_t current_queue_family = VK_QUEUE_FAMILY_IGNORED, uint32_t new_queue_family = VK_QUEUE_FAMILY_IGNORED);
    
    /**
     * Create buffer view with given size and offset
     * @param format how to interpret data in view'
     * @param range size in bytes of the view, can be ignored to use the whole buffer
     * @param offset offset in bytes from buffer start
     */
    VkBufferView createView(VkFormat format, VkDeviceSize range = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    //Get buffer memory requirements
    VkMemoryRequirements getMemoryRequirements() const;

    //Get size of buffer in bytes
    VkDeviceSize getSize() const;

    operator const VkBuffer&() const;
};



/**
 * UniformBuffer
 *  - In addition to normal buffer data holds variable layout inside uniform buffer
 */
class UniformBuffer : public Buffer{
    MixedBufferLayout m_variables;
public:
    UniformBuffer(VkBuffer buffer, const VkBufferCreateInfo& info, const MixedBufferLayout& uniform_variables);
    //Get reference to layout of uniform buffer variables
    const MixedBufferLayout& getVariables() const;
};


/**
 * BufferMemoryObject
 *  - Allocates memory for all buffers in constructor array
 */
class BufferMemoryObject{
protected:
    VkDeviceMemory m_memory;
    //holds offset from start of memory for each buffer, and last element as memory size
    vector<uint32_t> m_buffer_offsets;
public:
    //Allocate memory with given properties for each buffer
    BufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties);
};


/**
 * SharedBufferMemoryObject
 *  - Holds memory for all buffers, automatically maps it, so it can be uploaded without vulkan functions
 */
class SharedBufferMemoryObject : public BufferMemoryObject{
    //mapped memory pointer
    void* m_data;
public:
    //Allocate memory for given buffers. memory_properties must contain VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT for mapping to work
    SharedBufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
    
    /**
     * Copy data to buffer.
     * @param buffer_index index into buffer array passed to constructor to copy data into
     * @param data pointer to bytes to copy
     * @param size number of bytes to copy
     * @param offset offset in target buffer
     */
    void copyToBuffer(int buffer_index, const void* data, VkDeviceSize size_bytes, VkDeviceSize offset_in_buffer = 0);

    /**
     * Get a pointer to the memory of given buffer.
     * @param buffer_index index into buffer array passed to constructor to get pointer to
     */
    uint8_t* getMemoryPointer(int buffer_index = 0);

    /**
     * Flush memory (make changes visible to the GPU) in given range
     * @param buffer_index offset from start of buffer
     * @param length how many bytes to flush
     */
    void flushMemory(int buffer_index, VkDeviceSize size_bytes, VkDeviceSize offset_in_buffer = 0);

    /**
     * Flush memory (make changes visible to the GPU) in given range. Range starts with the first buffer.
     * @param size_bytes how many bytes to flush
     */
    void flushMemory(VkDeviceSize size_bytes);
};


/**
 * DeviceBufferMemoryObject
 *  - Allocates device local memory for given buffers
 */
class DeviceLocalBufferMemoryObject : public BufferMemoryObject{
public:
    DeviceLocalBufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
};

#endif