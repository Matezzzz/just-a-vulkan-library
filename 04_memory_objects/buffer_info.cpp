#include "buffer_info.h"
#include "../01_device/allocator.h"


BufferInfo::BufferInfo(VkDeviceSize size_bytes, VkBufferUsageFlags usage, VkBufferCreateFlags flags) :
    //buffer can be shared by multiple queue families, but their indices have to be specified.
    //                                                                                        family index count, pointer to family indices                                    
    m_info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, flags, size_bytes, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr}
{}
void BufferInfo::setSize(VkDeviceSize size){
    m_info.size = size;
}
Buffer BufferInfo::create(){
    return Buffer(g_allocator.get().createBuffer(m_info), m_info);
}





UniformBufferInfo::UniformBufferInfo(const vector<BufferType>& variables, VkBufferUsageFlags usage, VkBufferCreateFlags flags) :
    BufferInfo(0, usage, flags), m_variable_layout(variables)
{
    //set buffer size to match the one of variables
    BufferInfo::setSize(m_variable_layout.getSize());
}
UniformBuffer UniformBufferInfo::create(){
    //create uniform buffer, pass variables to the UniformBuffer object
    return UniformBuffer(g_allocator.get().createBuffer(m_info), m_info, m_variable_layout);
}



FloatBufferInfo::FloatBufferInfo(VkDeviceSize float_size, VkBufferUsageFlags usage, VkBufferCreateFlags flags) : 
    BufferInfo(float_size*sizeof(float), usage, flags)
{}

