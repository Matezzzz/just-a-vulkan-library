#ifndef LOCAL_OBJECT_CREATOR_H
#define LOCAL_OBJECT_CREATOR_H

#include "../01_device/device.h"
#include "../03_commands/command_buffer.h"
#include "../03_commands/synchronization.h"
#include "buffer_info.h"
#include "buffer.h"

/**
 * BufferUsageInfo
 *  - Describes data to upload to buffer, usage and creation flags
 *  - T is type of data in the buffer
 */
template<typename T>
class BufferUsageInfo{
public:
    const vector<T>& data;
    VkBufferUsageFlags usage;
    VkBufferCreateFlags create_flags;
    BufferUsageInfo(const vector<T>& data_, VkBufferUsageFlags usage_, VkBufferCreateFlags create_flags_ = 0) :
        data(data_), usage(usage_), create_flags(create_flags_)
    {}
};

class Queue;
class Image;


/**
 * LocalObjectCreator
 *  - class responsible for uploading data to buffers and images on the GPU
 *  - can create buffers as well
 */
class LocalObjectCreator{
    //size of staging buffer in bytes
    VkDeviceSize m_staging_buffer_size;
    //buffer in the shared memory - data is uploaded into it, and from it, to the GPU
    Buffer m_staging_buffer;
    //memory object for the staging buffer
    SharedBufferMemoryObject m_staging_buffer_memory;
    //command buffer for transfer command
    CommandBuffer m_transfer_command_buffer;
    //the queue to upload transfers to
    Queue& m_transfer_queue;
public:
    /**
     * Create a new LocalObjectCreator.
     * @param transfer_queue the queue to use for transfering data between cpu and gpu
     * @param staging_buffer_size the size, in bytes, to use for staging buffer. 1024 by default, good to increase if large textures or buffers will be copied.
     */
    LocalObjectCreator(Queue& transfer_queue, VkDeviceSize staging_buffer_size = 1024u);

    /**
     * Copy buffer data to the GPU
     * @param data data to copy
     * @param device_local_buffer buffer to copy to
     */
    template<typename T>
    void copyToLocal(const vector<T>& data, Buffer& device_local_buffer){
        copyToLocal(data.data(), data.size(), device_local_buffer);
    }

    /**
     * Copy buffer data to the GPU
     * @param data_typed pointer to data. Has to be of same type as data
     * @param data_size how many elements of above data should be copied
     * @param device_local_buffer buffer to copy to
     */
    template<typename T>
    void copyToLocal(const T* data_typed, VkDeviceSize data_size, Buffer& device_local_buffer){
        //compute data size in bytes
        data_size *= sizeof(T);
        //interpret data as an array of bytes
        const uint8_t* data = reinterpret_cast<const uint8_t*>(data_typed);
        //Split data into multiple chunks, each one the size of staging buffer. Go through each chunk to copy:
        for (VkDeviceSize data_offset = 0; data_offset < data_size; data_offset += m_staging_buffer_size){
            //find end of range to copy - is either staging buffer size or end of buffer to copy from
            uint32_t data_end = std::min(data_offset + m_staging_buffer_size, data_size);
            
            //since staging buffer is in shared memory, simply copy data to it
            //          index of buffer to copy to
            m_staging_buffer_memory.copyToBuffer(0, &data[data_offset], data_end - data_offset, data_offset);
            
            //record command buffer for copying data with only one copy command
            m_transfer_command_buffer.startRecordPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
            //record copy command
            m_transfer_command_buffer.cmdCopyFromBuffer(m_staging_buffer, device_local_buffer, data_end - data_offset, 0, data_offset);
            m_transfer_command_buffer.endRecordPrimary();

            //create synchronization for submitting buffer
            SubmitSynchronization transfer_synchronization;
            transfer_synchronization.setEndFence(Fence());
            //submit command buffer to queue
            m_transfer_queue.submit(m_transfer_command_buffer, transfer_synchronization);
            //wait a while for the transfer to finish, print error if it took too long
            if (!transfer_synchronization.waitFor(A_SHORT_WHILE)) PRINT_ERROR("Vulkan buffer copy failed") 
        }
    }
    /**
     * Copy given data to an image on the GPU
     * @param data data to copy
     * @param device_local_image the image to copy into
     * @param state the state the image is now in
     * @param end_state the state the image should be in after copying
     */
    void copyToLocal(const vector<uint8_t>& data, Image& device_local_image, ImageState state, ImageState end_state);

    /**
     * Copy given data to an image on the GPU
     * @param data_bytes pointer to data to copy
     * @param data_size_bytes how many bytes to copy
     * @param device_local_image the image to copy into
     * @param state the state the image is now in
     * @param end_state the state the image should be in after copying
     */
    void copyToLocal(const uint8_t* data_bytes, uint32_t data_size_bytes, Image& device_local_image, ImageState state, ImageState end_state);


    /**
     * Given default usage and data parameters, create buffers according to parameters and return them in a vector
     * @param default_usage_flags all buffers that don't have usage specified will default to this one
     * @param data_vectors any amount of parameters of vector<T> or BufferUsageInfo<T> type
     *  - vector<T> contains raw data, default usage and no flags are used when creating buffer
     *  - BufferUsageInfo<T> contains vector<T>, usage and flags to be specified when instantiating class
     */
    template<typename... Ts>
    vector<Buffer> createBuffers(VkBufferUsageFlags default_usage_flags, const Ts&... data_vectors){
        //allocate space for buffers
        vector<Buffer> buffers;
        buffers.reserve(sizeof...(data_vectors));
        //create buffers with given data
        createBuffersInternal(default_usage_flags, buffers, data_vectors...);
        //return buffers
        return buffers;
    }
private:
    //this function is called when there are no more buffers to create, it allocates memory for all created buffers
    void createBuffersInternal(VkBufferUsageFlags, vector<Buffer>& buffers){
        DeviceLocalBufferMemoryObject mem(buffers);
    }

    //Convert given vector to buffer usage info and call the second function to create current buffer
    template<typename T, typename... Ts>
    void createBuffersInternal(VkBufferUsageFlags default_usage_flags, vector<Buffer>& buffers, const vector<T>& data, const Ts&... other_data_vectors){
        createBuffersInternal(default_usage_flags, buffers, BufferUsageInfo<T>{data, default_usage_flags}, other_data_vectors...);
    }

    //create buffer based on data, then add it to buffers, and continue creating others
    template<typename T, typename... Ts>
    void createBuffersInternal(VkBufferUsageFlags default_usage_flags, vector<Buffer>& buffers, const BufferUsageInfo<T>& data, const Ts&... other_data_vectors){
        //current buffer index
        uint32_t buffer_i = buffers.size();
        //create buffer from current info. VK_BUFFER_USAGE_TRANSFER_DST_BIT flag is enabled to make it possible to copy given data to buffer later.
        buffers.push_back(BufferInfo{data.data, data.usage, data.create_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT}.create());
        //create remaining buffers
        createBuffersInternal(default_usage_flags, buffers, other_data_vectors...);
        //after all buffers are created, they have memory bound to them already. Now given data can be copied to the buffer
        copyToLocal(data.data, buffers[buffer_i]);
    }
};


#endif