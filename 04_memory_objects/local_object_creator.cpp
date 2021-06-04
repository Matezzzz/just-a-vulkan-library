#include "local_object_creator.h"

#include "../03_commands/command_buffer.h"
#include "../03_commands/command_pool.h"
#include "image.h"


LocalObjectCreator::LocalObjectCreator(Queue& transfer_queue, VkDeviceSize staging_buffer_size) : m_staging_buffer_size(staging_buffer_size),
    m_staging_buffer(BufferInfo(staging_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT).create()), m_staging_buffer_memory({m_staging_buffer}),
    //command pool - make buffers individually free-able
    m_transfer_command_buffer(CommandPoolInfo{transfer_queue.getFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT}.create().allocateBuffer()),
    m_transfer_queue(transfer_queue)
{}

void LocalObjectCreator::copyToLocal(const vector<uint8_t>& data, Image& device_local_image, ImageState state, ImageState end_state){
    copyToLocal(data.data(), data.size(), device_local_image, state, end_state);
}
void LocalObjectCreator::copyToLocal(const uint8_t* data_bytes, uint32_t data_size_bytes, Image& device_local_image, ImageState state, ImageState end_state){
    //images cannot be copied as multiple parts - if the size to copy is larger than staging buffer, print error
    if (data_size_bytes > m_staging_buffer_size) PRINT_ERROR("Trying to copy data of larger size than staging buffer. Staging buffer size: " << m_staging_buffer_size << ", data size: " << data_size_bytes);
    //if the data to copy and target image have different sizes, print warning
    if (data_size_bytes != device_local_image.getSizeInBytes()) PRINT_WARN("The data to copy and the target image have different sizes. Data size: " << data_size_bytes << ", Image size: " << device_local_image.getSizeInBytes()) 
    //copy data to staging buffer
    m_staging_buffer_memory.copyToBuffer(0, data_bytes, data_size_bytes);

    //record command buffer for transfer
    m_transfer_command_buffer.startRecordPrimary(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    //copy to image command
    m_transfer_command_buffer.cmdCopyToTexture(m_staging_buffer, device_local_image, state, end_state);
    m_transfer_command_buffer.endRecord();

    //synchronization for copy operation with one end fence
    SubmitSynchronization transfer_synchronization;
    transfer_synchronization.setEndFence(Fence());
    //submit task to the queue and wait for it to finish
    m_transfer_queue.submit(m_transfer_command_buffer, transfer_synchronization);
    //wait for it to finish
    transfer_synchronization.waitFor(A_SHORT_WHILE);
    //reset recorded buffer
    m_transfer_command_buffer.resetBuffer(false);
}
