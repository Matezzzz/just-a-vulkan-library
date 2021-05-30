#include "command_buffer.h"
#include "../04_memory_objects/buffer.h"
#include "../04_memory_objects/image.h"
#include "../06_render_passes/renderpass.h"
#include "../07_shaders/pipelines_context.h"
#include "../08_pipeline/pipeline.h"


CommandBuffer::CommandBuffer(VkCommandBuffer buffer) : m_buffer(buffer)
{}
void CommandBuffer::startRecordPrimary(VkCommandBufferUsageFlags usage){
    //type, next, usage, inheritance info(used only for secondary buffers)
    VkCommandBufferBeginInfo begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, usage, nullptr};
    //begin recording the buffer
    VkResult result = vkBeginCommandBuffer(m_buffer, &begin_info);
    DEBUG_CHECK("Start primary command buffer recording", result)
}
void CommandBuffer::endRecordPrimary(){
    //end recording the buffer
    VkResult result = vkEndCommandBuffer(m_buffer);
    DEBUG_CHECK("End primary command buffer recording", result)
}
void CommandBuffer::resetBuffer(bool release_resources){
    VkResult result = vkResetCommandBuffer(m_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
    DEBUG_CHECK("Reset command buffer", result)
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, vector<VkBufferMemoryBarrier> memory_barriers){
    //record buffer memory barriers
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        memory_barriers.size(), &memory_barriers[0], 
        0, nullptr);    //no image barriers
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, vector<VkImageMemoryBarrier> memory_barriers){
    //record image memory barriers
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        0, nullptr,     //no buffer barriers
        memory_barriers.size(), &memory_barriers[0]);
}
void CommandBuffer::cmdCopyFromBuffer(const Buffer& from, const Buffer& to, VkDeviceSize size, VkDeviceSize from_offset, VkDeviceSize to_offset){
    //if the whole size should be copied, get actual size from source buffer
    if (size == VK_WHOLE_SIZE) size = from.getSize();
    //if target memory is invalid (out of bounds), print error and return
    if (to_offset + size > to.getSize()){
        PRINT_ERROR("Copying outside of buffer boundaries")
        return;
    }
    //record command to copy data from one buffer to another
    VkBufferCopy copy{from_offset, to_offset, size};
    vkCmdCopyBuffer(m_buffer, from, to, 1, &copy);
}
void CommandBuffer::cmdCopyToTexture(const Buffer& from, Image& texture, VkImageLayout end_layout, VkAccessFlags end_access)
{
    //if the image isn't in the correct layout already, record a memory barrier to change the layout
    if (texture.getLayout() != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        cmdBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,\
            {texture.createMemoryBarrier(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT)});
    }
    //values - buffer offset, buffer_row_length(0 for tightly packed), buffer_image_height(0 for tightly packed)
    // - VkImageSubresourceLayers - aspect, mipmap level to copy to, base array layer to copy to, number of layers to copy to
    // - VkOffset3D - offset in image to copy into
    // - size of image volume to copy into
    VkBufferImageCopy copy{0, 0, 0, VkImageSubresourceLayers{texture.getAspect(), 0, 0, 1}, VkOffset3D{0, 0, 0}, texture.getSize()};
    vkCmdCopyBufferToImage(m_buffer, from, texture, texture.getLayout(), 1, &copy);
    //if layout needs to be transitioned after end, record the layout transition
    if (end_layout != VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL){
        cmdBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, {texture.createMemoryBarrier(end_layout, end_access)});
    }
}
void CommandBuffer::cmdClearColor(const Image& image, VkClearColorValue color){
    //range - all mipmaps, all array layers
    VkImageSubresourceRange range{image.getAspect(), 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS};
    //record filling the image with color
    vkCmdClearColorImage(m_buffer, image, image.getLayout(), &color, 1, &range);
}
void CommandBuffer::cmdBeginRenderPass(RenderPassSettings& settings, VkRenderPass render_pass, VkFramebuffer framebuffer){
    //INLINE - renderpass contains no secondary command buffers
    vkCmdBeginRenderPass(m_buffer, &settings.getBeginInfo(render_pass, framebuffer), VK_SUBPASS_CONTENTS_INLINE);
}
void CommandBuffer::cmdBindPipeline(const Pipeline& pipeline){
    vkCmdBindPipeline(m_buffer, pipeline.getBindPoint(), pipeline);
}
void CommandBuffer::cmdBindPipeline(const Pipeline& pipeline, VkDescriptorSet set){
    //bind pipeline, then it's descriptor set
    cmdBindPipeline(pipeline);
    cmdBindSet(pipeline, set);
}
void CommandBuffer::cmdBindPipeline(const Pipeline& pipeline, const vector<VkDescriptorSet>& sets){
    //bind pipeline, then it's descriptor sets
    cmdBindPipeline(pipeline);
    cmdBindSets(pipeline, sets);
}
void CommandBuffer::cmdBindSet(const Pipeline& pipeline, VkDescriptorSet set){
    //record binding given descriptor sets                                          0 -> no offset; 1 -> set count;  (0, nullptr) -> no dynamic offsets 
    vkCmdBindDescriptorSets(m_buffer, pipeline.getBindPoint(), pipeline.getLayout(), 0, 1, &set, 0, nullptr);
}
void CommandBuffer::cmdBindSets(const Pipeline& pipeline, const vector<VkDescriptorSet>& descriptor_sets){
    //record binding given descriptor sets                                                     0 -> no offset                                    (0, nullptr) -> no dynamic offsets 
    vkCmdBindDescriptorSets(m_buffer, pipeline.getBindPoint(), pipeline.getLayout(), 0, descriptor_sets.size(), descriptor_sets.data(), 0, nullptr);
}
void CommandBuffer::cmdBindVertexBuffers(vector<VkBuffer> buffers, uint32_t binding_offset){
    //record binding given buffers
    vector<VkDeviceSize> buffer_offsets(buffers.size(), 0);
    vkCmdBindVertexBuffers(m_buffer, binding_offset, buffers.size(), buffers.data(), buffer_offsets.data());
}
void CommandBuffer::cmdBindVertexBuffers(vector<VkBuffer> buffers, vector<VkDeviceSize> buffer_offsets, uint32_t binding_offset){
    vkCmdBindVertexBuffers(m_buffer, binding_offset, buffers.size(), buffers.data(), buffer_offsets.data());
}
void CommandBuffer::cmdPushConstants(const Pipeline& pipeline, const PushConstantData& push_consts){
    //                                                                          0 -> no offset
    vkCmdPushConstants(m_buffer, pipeline.getLayout(), push_consts.getStages(), 0, push_consts.size(), push_consts.data());
}
void CommandBuffer::cmdDrawVertices(uint32_t vertex_count, uint32_t vertex_offset)
{
    //                                1 -> 1 instance   0 -> no instance offset
    vkCmdDraw(m_buffer, vertex_count, 1, vertex_offset, 0);
}
void CommandBuffer::cmdDrawInstances(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset){
    vkCmdDraw(m_buffer, vertex_count, instance_count, vertex_offset, instance_offset);
}
void CommandBuffer::cmdEndRenderPass(){
    vkCmdEndRenderPass(m_buffer);
}
void CommandBuffer::cmdDispatchCompute(uint32_t size_x, uint32_t size_y, uint32_t size_z){
    vkCmdDispatch(m_buffer, size_x, size_y, size_z);
}
VkCommandBuffer CommandBuffer::get(){
    return m_buffer;
}
CommandBuffer::operator VkCommandBuffer() const{
    return m_buffer;
}
