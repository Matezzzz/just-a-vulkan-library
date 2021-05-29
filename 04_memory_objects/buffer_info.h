#ifndef BUFFER_INFO_H
#define BUFFER_INFO_H

#include "../00_base/vulkan_base.h"
#include "buffer.h"


/**
 * Class holding info for creating a VkBuffer.
 */
class BufferInfo
{
protected:
    VkBufferCreateInfo m_info;
public:
    /**
     * Create new buffer info with the given size, given usage and flags.
     * @param size the number of bytes to be stored in the buffer.
     * @param usage how the buffer is used. Possible values - VK_BUFFER_USAGE_***
     * @param flags optional, additional buffer creation flags. Possible values - VK_BUFFER_CREATE_***
     */
    BufferInfo(VkDeviceSize size_bytes, VkBufferUsageFlags usage, VkBufferCreateFlags flags = 0);
    /**
     * Create a buffer that can hold the data passed in.
     * @param data  the data to be held
     * @param usage how the buffer is used. Possible values - VK_BUFFER_USAGE_***
     * @param flags optional, additional buffer creation flags. Possible values - VK_BUFFER_CREATE_***
     */
    template<typename T>
    BufferInfo(const vector<T>& data, VkBufferUsageFlags usage, VkBufferCreateFlags flags = 0) : BufferInfo{data.size() * sizeof(T), usage, flags}
    {}
    /**
     * Create a buffer that can hold size_elements of given type.
     * @param size_elements number of elements to be in the buffer
     * @param usage how the buffer is used. Possible values - VK_BUFFER_USAGE_***
     * @param flags optional, additional buffer creation flags. Possible values - VK_BUFFER_CREATE_***
     */
    template<typename T>
    BufferInfo(VkDeviceSize size_elements, VkBufferUsageFlags usage, VkBufferCreateFlags flags = 0) : BufferInfo(size_elements * sizeof(T), usage, flags)
    {}
    /**
     * Used to set size in bytes before creating buffer.
     * @param size the number of bytes to be stored in the buffer
     */
    void setSize(VkDeviceSize size);
    /**
     * Create a buffer object from this info.
     */
    Buffer create();
};



class UniformBufferInfo : public BufferInfo
{
    MixedBufferLayout m_variable_layout;
public:
    /**
     * Create a new uniform buffer info, given the variables that will be present, usage and flags.
     * @param variables vector of all variables present in this uniform buffer, in the same order as in shader.
     * @param usage how the buffer is used, is only VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT by default. Possible values - VK_BUFFER_USAGE_***
     * @param flags optional, additional buffer creation flags. Possible values - VK_BUFFER_CREATE_***
     */
    UniformBufferInfo(const vector<BufferType>& variables, VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VkBufferCreateFlags flags = 0);
    /**
     * Create an uniform buffer object from this info.
     */
    UniformBuffer create();
};



/**
 * Buffer info with float data type.
 */
class FloatBufferInfo : public BufferInfo{
public:
    /**
     * Create new buffer info with size float_size*sizeof(float), given usage and flags.
     * @param float_size the number of floats to be stored in the buffer.
     * @param usage how the buffer is used. Possible values - VK_BUFFER_USAGE_***
     * @param flags optional, additional buffer creation flags. Possible values - VK_BUFFER_CREATE_***
     */
    FloatBufferInfo(VkDeviceSize float_size, VkBufferUsageFlags usage, VkBufferCreateFlags flags = 0);
};

#endif