#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H


#include "../00_base/vulkan_base.h"

class Buffer;
class Image;
class RenderPassSettings;
class Pipeline;
class PushConstantData;


/**
 * CommandBuffer
 *  - Contains all functions for recording a command buffer
 */
class CommandBuffer{
    VkCommandBuffer m_buffer;
public:
    CommandBuffer(VkCommandBuffer buffer);

    /**
     * Start recording the command buffer
     * @param usage accepted flags - most common VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, other VK_COMMAND_BUFFER_USAGE_***
     */
    void startRecordPrimary(VkCommandBufferUsageFlags usage = 0);
    //End recording of the command buffer
    void endRecordPrimary();
    /**
     * Reset the command buffer
     * @param release_resources if true, all resources(memory) that the buffer occupied will be freed as well, otherwise the buffer can be recorded again into the same space
     */
    void resetBuffer(bool release_resources);
    
    /**
     * Insert given memory barriers into the buffer.
     * @param past_stages the stage that has to be finished for the memory barrier to execute
     * @param next_stages the stage at which the barrier will wait for past_stages to finish
     * @param memory_barriers the memory barriers specifying which buffers the memory barriers are bound to
     */
    void cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, vector<VkBufferMemoryBarrier> memory_barriers);
    
    /**
     * Insert given memory barriers into the buffer.
     * @param past_stages the stage that has to be finished for the memory barrier to execute
     * @param next_stages the stage at which the barrier will wait for past_stages to finish
     * @param memory_barriers the memory barriers specifying which images the memory barriers are bound to, and how to change layouts and access flags of each image
     */
    void cmdBarrier(VkPipelineStageFlags past_stages, VkPipelineStageFlags next_stages, vector<VkImageMemoryBarrier> memory_barriers);

    //Fill image with given color. Image must have VK_IMAGE_TRANSFER_DST flag on    
    void cmdClearColor(const Image& image, VkClearColorValue color);

    /**
     * Copy data from one buffer to another
     * @param from source buffer
     * @param to target buffer
     * @param size number of bytes to copy
     * @param from_offset offset in source buffer
     * @param to_offset offset in target buffer
     */
    void cmdCopyFromBuffer(const Buffer& from, const Buffer& to, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize from_offset = 0, VkDeviceSize to_offset = 0);

    /**
     * Copy data from buffer to image
     * @param from source buffer
     * @param to target image
     * @param end_layout image layout after copying
     * @param end_access access flags after copying
     */
    void cmdCopyToTexture(const Buffer& from, Image& to, VkImageLayout end_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VkAccessFlags end_access = 0);

    /**
     * Begin renderpass
     * @param settings begin info and clear colors
     * @param render_pass
     * @param framebuffer
     */
    void cmdBeginRenderPass(RenderPassSettings& settings, VkRenderPass render_pass, VkFramebuffer framebuffer);

    //Bind given pipeline
    void cmdBindPipeline(const Pipeline& pipeline);

    //Bind given pipeline with given descriptor sets
    void cmdBindPipeline(const Pipeline& pipeline, const vector<VkDescriptorSet>& sets);

    //Bind given descriptor sets for use with given pipeline 
    void cmdBindSets(const Pipeline& pipeline, const vector<VkDescriptorSet>& descriptor_sets);
    
    /**
     * Bind given vertex buffers
     * @param buffers
     * @param binding_offset offset into internal buffer array
     */
    void cmdBindVertexBuffers(vector<VkBuffer> buffers, uint32_t binding_offset = 0);

    /**
     * Bind given vertex buffers
     * @param buffers
     * @param buffer_offsets offsets from start of each buffer
     * @param binding_offset offset into internal buffer array
     */
    void cmdBindVertexBuffers(vector<VkBuffer> buffers, vector<VkDeviceSize> buffer_offsets, uint32_t binding_offset = 0);

    /**
     * Upload given push constant data
     * @param pipeline
     * @param push_consts push constant data to push
     */
    void cmdPushConstants(const Pipeline& pipeline, const PushConstantData& push_consts);

    /**
     * Draw given amount of vertices
     * @param vertex_count how many to draw
     * @param vertex_offset index of vertex to start from
     */
    void cmdDrawVertices(uint32_t vertex_count, uint32_t vertex_offset = 0);

    /**
     * Draw given amount of vertices for each instance.
     * @param vertex_count how many vertices per instance
     * @param instance_count how many instances
     * @param vertex_offset index of vertex to start from
     * @param instance_offset index of instance to start from
     */
    void cmdDrawInstances(uint32_t vertex_count, uint32_t instance_count, uint32_t vertex_offset = 0, uint32_t instance_offset = 0);

    //End current render pass
    void cmdEndRenderPass();

    /**
     * Run compute pipeline with given work group count
     * Take note the following values reflect the amount of times the local workgroup runs - it's size is specified in each respective compute shader. The total amount of compute shaders dispatched per dimension is equal to (local_size * size).
     * @param size_x the count of work groups in the x dimension
     * @param size_y the count of work groups in the y dimension
     * @param size_z the count of work groups in the z dimension
     */
    void cmdDispatchCompute(uint32_t size_x, uint32_t size_y = 1, uint32_t size_z = 1);

    VkCommandBuffer get();
    operator VkCommandBuffer() const;
};

#endif