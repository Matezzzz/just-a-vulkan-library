#include "buffer.h"

#include "../01_device/allocator.h"

Buffer::Buffer() : m_buffer(VK_NULL_HANDLE), m_size(0)
{}
Buffer::Buffer(VkBuffer buffer, const VkBufferCreateInfo& info) : m_buffer(buffer), m_size(info.size)
{}
VkBufferMemoryBarrier Buffer::createMemoryBarrier(VkAccessFlags current_access, VkAccessFlags new_access, uint32_t current_queue_family, uint32_t new_queue_family)
{
    //fill the memory barrier structure
    VkBufferMemoryBarrier barrier{
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
        current_access, new_access, 
        current_queue_family, new_queue_family,
        m_buffer,
        0, VK_WHOLE_SIZE //offset, size
    };
    return barrier;
}
VkBufferView Buffer::createView(VkFormat format, VkDeviceSize range, VkDeviceSize offset){
    //if offset is out of bounds
    if (offset >= m_size) DEBUG_ERROR("Buffer offset out of bounds")
    //if the view would be out of buffer bounds, print error
    if (range != VK_WHOLE_SIZE && offset + range > m_size) DEBUG_ERROR("The buffer view is created with range outside of actual buffer")
    //                                                                              no flags
    VkBufferViewCreateInfo info{VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO, nullptr, 0, m_buffer, format, offset, range};
    return g_allocator.get().createBufferView(info);
}
VkMemoryRequirements Buffer::getMemoryRequirements() const{
    //get memory requirements and return them
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(g_device, m_buffer, &requirements);
    return requirements;
}
bool Buffer::isValid() const{
    return m_buffer != VK_NULL_HANDLE;
}
VkDeviceSize Buffer::getSize() const{
    return m_size;
}
Buffer::operator const VkBuffer&() const {
    return m_buffer;
}


UniformBuffer::UniformBuffer(VkBuffer buffer, const VkBufferCreateInfo& info, const MixedBufferLayout& uniform_variables) :\
    Buffer(buffer, info), m_variables(uniform_variables)
{}
const MixedBufferLayout& UniformBuffer::getVariables() const{
    return m_variables;
}



BufferMemoryObject::BufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties) :
    m_buffer_offsets(buffers.size() + 1)
{
    //buffer offsets must be multiples of this value
    VkDeviceSize buffer_offset_multiplier = g_allocator.get().getLimits().nonCoherentAtomSize;

    
    m_buffer_offsets[0] = 0;
    //all memory types that can be used
    uint32_t memory_type_bits = 0xFFFFFFFF;    
    VkMemoryRequirements memory_requirements;
    //go through all the buffers to allocate memory for
    for (uint32_t i = 0; i < buffers.size(); i++){
        //get memory requirements for current buffer
        memory_requirements = buffers[i].getMemoryRequirements();
        //eliminate all memory types unusable for current buffer
        memory_type_bits &= memory_requirements.memoryTypeBits;
        //Compute offset of next buffer - add padding to make the buffer offset align with memory blocks
        m_buffer_offsets[i + 1] = m_buffer_offsets[i] + roundUpToMemoryBlock(memory_requirements.size, buffer_offset_multiplier);
    }
    //allocate memory with given properties and correct type - last buffer offset is equal to size of all previous buffers, is passed as size
    m_memory = g_allocator.get().allocateMemory(m_buffer_offsets.back(), memory_type_bits, memory_properties);
    //bind allocated memory to each individual buffer
    for (uint32_t i = 0; i < buffers.size(); i++){
        VkResult result = vkBindBufferMemory(g_device, buffers[i], m_memory, m_buffer_offsets[i]);
        DEBUG_CHECK("Buffer memory binding", result);
    }  
}
BufferMemoryObject::BufferMemoryObject(const Buffer& buffer, VkMemoryPropertyFlags memory_properties) : BufferMemoryObject{vector<Buffer>{buffer}, memory_properties}
{}




SharedBufferMemoryObject::SharedBufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties)
    : BufferMemoryObject(buffers, memory_properties)
{
    //get a pointer to shared memory
    m_data = g_allocator.get().mapMemory(m_memory, 0, m_buffer_offsets.back());
}
void SharedBufferMemoryObject::copyToBuffer(int buffer_index, const void* data, VkDeviceSize size_bytes, VkDeviceSize offset_in_buffer){
    //copy given data to shared memory
    memcpy(reinterpret_cast<uint8_t*>(m_data) + m_buffer_offsets[buffer_index] + offset_in_buffer, data, size_bytes);
   
    flushMemory(buffer_index, size_bytes, offset_in_buffer);
}

uint8_t* SharedBufferMemoryObject::getMemoryPointer(int buffer_index){
    return reinterpret_cast<uint8_t*>(m_data) + m_buffer_offsets[buffer_index];
}


void SharedBufferMemoryObject::flushMemory(int buffer_index, VkDeviceSize size_bytes, VkDeviceSize offset_in_buffer){
    //flush the memory range - make sure the data is usable by the GPU for all following commands
    //both start and end values must be multiplies of memory block size, take nearest before and after copied range as boundary points
    VkDeviceSize memory_block_size = g_allocator.get().getLimits().nonCoherentAtomSize;
    VkMappedMemoryRange mapped_memory{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE, nullptr, m_memory,
        roundDownToMemoryBlock((uint64_t) m_buffer_offsets[buffer_index] + offset_in_buffer, memory_block_size), roundUpToMemoryBlock(size_bytes, memory_block_size)};
    VkResult result = vkFlushMappedMemoryRanges(g_device, 1, &mapped_memory);
    DEBUG_CHECK("Memory flush", result)
}
void SharedBufferMemoryObject::flushMemory(VkDeviceSize size_bytes){
    flushMemory(0, size_bytes);
}




DeviceLocalBufferMemoryObject::DeviceLocalBufferMemoryObject(const vector<Buffer>& buffers, VkMemoryPropertyFlags memory_properties) :
    BufferMemoryObject(buffers, memory_properties)
{}
