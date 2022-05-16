#include "command_buffer.h"
#include "../04_memory_objects/buffer.h"
#include "../04_memory_objects/image.h"
#include "../06_render_passes/renderpass.h"
#include "../07_shaders/pipelines_context.h"
#include "../08_pipeline/pipeline.h"





CommandBufferInheritanceInfo::CommandBufferInheritanceInfo() : m_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO, nullptr, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, false, 0, 0}
{}
CommandBufferInheritanceInfo& CommandBufferInheritanceInfo::setRenderPass(VkRenderPass pass, int subpass_index){
    m_info.renderPass = pass;
    m_info.subpass = subpass_index;
    return *this;
}
CommandBufferInheritanceInfo& CommandBufferInheritanceInfo::setFramebuffer(VkFramebuffer framebuffer){
    m_info.framebuffer = framebuffer;
    return *this;
}
CommandBufferInheritanceInfo& CommandBufferInheritanceInfo::useOcclusionQuery(bool enable, VkQueryControlFlags query_flags, VkQueryPipelineStatisticFlags pipeline_stats){
    m_info.occlusionQueryEnable = enable;
    m_info.queryFlags = query_flags;
    m_info.pipelineStatistics = pipeline_stats;
    return *this;
}
CommandBufferInheritanceInfo::operator const VkCommandBufferInheritanceInfo&() const{
    return m_info;
}




CommandBuffer::CommandBuffer(VkCommandBuffer buffer) : m_buffer(buffer)
{}
void CommandBuffer::startRecordPrimary(VkCommandBufferUsageFlags usage){
    //type, next, usage, inheritance info(used only for secondary buffers)
    VkCommandBufferBeginInfo begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, usage, nullptr};
    //begin recording the buffer
    VkResult result = vkBeginCommandBuffer(m_buffer, &begin_info);
    DEBUG_CHECK("Start primary command buffer recording", result)
}
void CommandBuffer::startRecordSecondary(const VkCommandBufferInheritanceInfo& inh_info, VkCommandBufferUsageFlags usage){
    //create begin info with inheritance info included
    VkCommandBufferBeginInfo begin_info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, nullptr, usage, &inh_info};
    //begin the recording
    VkResult result = vkBeginCommandBuffer(m_buffer, &begin_info);
    DEBUG_CHECK("Start secondary command buffer recording", result)
}
void CommandBuffer::endRecord(){
    //end recording the buffer
    VkResult result = vkEndCommandBuffer(m_buffer);
    DEBUG_CHECK("End command buffer recording", result)
}
void CommandBuffer::resetBuffer(bool release_resources){
    VkResult result = vkResetCommandBuffer(m_buffer, release_resources ? VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT : 0);
    DEBUG_CHECK("Reset command buffer", result)
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, const VkBufferMemoryBarrier& memory_barrier){
    //record buffer memory barriers
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        1, &memory_barrier, 
        0, nullptr);    //no image barriers
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, const vector<VkBufferMemoryBarrier>& memory_barriers){
    //record buffer memory barriers
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        memory_barriers.size(), memory_barriers.data(), 
        0, nullptr);    //no image barriers
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, const VkImageMemoryBarrier& memory_barrier){
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        0, nullptr,     //no buffer barriers
        1, &memory_barrier);
}
void CommandBuffer::cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, const vector<VkImageMemoryBarrier>& memory_barriers){
    //record image memory barriers
    vkCmdPipelineBarrier(m_buffer, past_stages, next_stages, 0,
        0, nullptr,     //no memory barriers
        0, nullptr,     //no buffer barriers
        memory_barriers.size(), memory_barriers.data());
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
void CommandBuffer::cmdCopyToTexture(const Buffer& from, Image& texture, ImageState state, ImageState end_state)
{
    ImageState transfer_state = ImageState(IMAGE_TRANSFER_DST);
    //if the image isn't in the correct layout already, record a memory barrier to change the layout
    if (state != transfer_state){
        cmdBarrier(VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, texture.createMemoryBarrier(state, ImageState(IMAGE_TRANSFER_DST)));
    }
    //values - buffer offset, buffer_row_length(0 for tightly packed), buffer_image_height(0 for tightly packed)
    // - VkImageSubresourceLayers - aspect, mipmap level to copy to, base array layer to copy to, number of layers to copy to
    // - VkOffset3D - offset in image to copy into
    // - size of image volume to copy into
    VkBufferImageCopy copy{0, 0, 0, VkImageSubresourceLayers{texture.getAspect(), 0, 0, 1}, VkOffset3D{0, 0, 0}, texture.getSize()};
    vkCmdCopyBufferToImage(m_buffer, from, texture, transfer_state.layout, 1, &copy);
    //if layout needs to be transitioned after end, record the layout transition
    if (end_state != transfer_state){
        cmdBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, texture.createMemoryBarrier(transfer_state, end_state));
    }
}
void CommandBuffer::cmdClearColor(const Image& image, ImageState state, VkClearColorValue color){
    //range - all mipmaps, all array layers
    VkImageSubresourceRange range{image.getAspect(), 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS};
    //record filling the image with color
    vkCmdClearColorImage(m_buffer, image, state.layout, &color, 1, &range);
}
void CommandBuffer::cmdExecuteCommands(VkCommandBuffer buffer){
    cmdExecuteCommands(&buffer, 1);
}
void CommandBuffer::cmdExecuteCommands(const VkCommandBuffer* buffers, uint32_t count){
    //execute all buffers
    vkCmdExecuteCommands(m_buffer, count, buffers);
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
void CommandBuffer::cmdBindVertexBuffer(VkBuffer buffer, uint32_t binding_offset){
    //offset in buffer
    VkDeviceSize a = 0;
    //bind given buffer
    vkCmdBindVertexBuffers(m_buffer, binding_offset, 1, &buffer, &a);
}
void CommandBuffer::cmdBindVertexBuffers(vector<VkBuffer> buffers, uint32_t binding_offset){
    //record binding given buffers
    vector<VkDeviceSize> buffer_offsets(buffers.size(), 0);
    vkCmdBindVertexBuffers(m_buffer, binding_offset, buffers.size(), buffers.data(), buffer_offsets.data());
}
void CommandBuffer::cmdBindVertexBuffers(vector<VkBuffer> buffers, vector<VkDeviceSize> buffer_offsets, uint32_t binding_offset){
    vkCmdBindVertexBuffers(m_buffer, binding_offset, buffers.size(), buffers.data(), buffer_offsets.data());
}
void CommandBuffer::cmdBindIndexBuffer(VkBuffer buffer, VkIndexType data_type, VkDeviceSize offset){
    vkCmdBindIndexBuffer(m_buffer, buffer, offset, data_type);
}
void CommandBuffer::cmdPushConstants(const Pipeline& pipeline, const PushConstantData& push_consts){
    //                                                                          0 -> no offset
    vkCmdPushConstants(m_buffer, pipeline.getLayout(), push_consts.getStages(), 0, push_consts.size(), push_consts.data());
}
void CommandBuffer::cmdDrawVertices(uint32_t vertex_count, uint32_t vertex_offset)
{
    cmdDrawInstances(vertex_count, 1, vertex_offset, 0);
}
void CommandBuffer::cmdDrawInstances(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset, uint32_t instance_offset){
    vkCmdDraw(m_buffer, vertex_count, instance_count, vertex_offset, instance_offset);
}
void CommandBuffer::cmdDrawVerticesIndexed(uint32_t index_count, uint32_t index_offset, int32_t index_shift){
    cmdDrawInstancesIndexed(index_count, 1, index_offset, 0, index_shift);
}

void CommandBuffer::cmdDrawInstancesIndexed(uint32_t index_count, uint32_t instance_count, uint32_t index_offset, uint32_t instance_offset, int32_t index_shift){
    vkCmdDrawIndexed(m_buffer, index_count, instance_count, index_offset, index_shift, instance_offset);
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
