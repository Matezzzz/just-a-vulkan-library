#include "local_object_creator.h"

#include "../03_commands/command_buffer.h"
#include "../03_commands/command_pool.h"
#include "image.h"


void StagingBuffer::transfer(CommandBuffer& buf, const Buffer& buffer, uint32_t dst_offset){
    buf.cmdCopyBuffer(*this, buffer, m_current_data_size, dst_offset);
}
void StagingBuffer::transfer(CommandBuffer& buf, const Image& image){
    buf.cmdCopyBufferToImage(*this, image);
}
void StagingBuffer::transferWithTransitions(CommandBuffer& buf, const Image& image, ImageState begin_state, ImageState end_state){
    buf.cmdCopyToTexture(*this, image, begin_state, end_state);
}








LocalObjectCreator::LocalObjectCreator(Queue& transfer_queue, VkDeviceSize staging_buffer_size) :
    m_staging_buffer(staging_buffer_size),
    //command pool - make buffers individually free-able
    m_transfer_command_buffer(CommandPoolInfo{transfer_queue.getFamilyIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT}.create().allocateBuffer()),
    m_transfer_queue(transfer_queue)
{}

void LocalObjectCreator::copyToLocal(const vector<uint8_t>& data, Image& device_local_image, ImageState state, ImageState end_state){
    copyToLocal(data.data(), data.size(), device_local_image, state, end_state);
}
void LocalObjectCreator::copyToLocal(const uint8_t* data_bytes, uint32_t data_size_bytes, Image& device_local_image, ImageState state, ImageState end_state){
    //if the data to copy and target image have different sizes, print warning
    if (data_size_bytes != device_local_image.getSizeInBytes()) PRINT_WARN("The data to copy and the target image have different sizes. Data size: " << data_size_bytes << ", Image size: " << device_local_image.getSizeInBytes()) 
    //copy data to the staging buffer
    m_staging_buffer.copyTo(data_bytes, data_size_bytes);

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
