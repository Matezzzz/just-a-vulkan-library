#ifndef FLOW_SECTIONS_BASE_H
#define FLOW_SECTIONS_BASE_H

#include "../00_base/vulkan_base.h"
#include "../04_memory_objects/buffer.h"
#include "../04_memory_objects/image.h"
#include "../07_shaders/pipelines_context.h"
#include <glm/glm.hpp>
#include <map>


using std::map;

/**
 * Size3
 *  - Holds 3 uint components, represents a 3D cube size
 */
struct Size3 : public glm::uvec3{
    using glm::uvec3::uvec3;
    /**
     * Construct size from uvec3
     */
    Size3(const glm::uvec3& v);

    /**
     * Return volume of cube represented by this size (= x * y * z)
     */
    uint32_t volume() const;
};



/**
 * PipelineBufferState
 *  - Holds BufferState and the last time the buffer was used
 */
class PipelineBufferState : public BufferState{
public:
    //last flags, with which the buffer was used
    VkPipelineStageFlags last_use;

    /**
     * Construct PipelineBufferState with no last use and no state(access flags = 0)
     */
    PipelineBufferState();

    /**
     * Construct PipelineBufferState with given state and last use
     * @param state current state the buffer is in
     * @param last_use last flags, with which buffer was used
     */
    PipelineBufferState(BufferState state, VkPipelineStageFlags last_use);
    
};



/**
 * PipelineImageState
 *  - Holds ImageState and the last stages in which the image was used
 */
class PipelineImageState : public ImageState{
public:
    //last flags with which the image was used
    VkPipelineStageFlags last_use;
    //true if there already was a barrier transitioning to current use
    bool barrier_done;
    
    /**
     * Construct PipelineImageState with no last use, no last layout and access
     */
    PipelineImageState();

    /**
     * Construct PipelineImageState with given state and last use
     * @param state the state the image is currently in
     * @param last_use the last flags with which it was used in a pipeline
     */
    PipelineImageState(ImageState state, VkPipelineStageFlags last_use = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, bool barrier_done = false);
};



/**
 * FlowContext
 *  - Holds all images and buffers to be used by flow sections, + their current states
 */
class FlowContext{
    //vector of all images used by flow sections
    vector_ext<ExtImage> m_images;
    //states, one for each image
    vector<PipelineImageState> m_image_states;

    //vector of all buffers used by flow sections
    vector<Buffer> m_buffers;
    //states, one for each buffer
    vector<PipelineBufferState> m_buffer_states;

    //list of all registered vertex buffers. First = buffer index, second = buffer size in vertices
    map<uint32_t, uint32_t> m_vertex_buffer_sizes;

    //dictionary of all push constant data objects. Indexed by name of shader directory 
    map<string, PushConstantData*> m_push_constants;

    //how large must a staging buffer be to copy all required data in one operation
    VkDeviceSize m_max_copy_size = 0;
public:
    /**
     * Create FlowContext without any descriptors.
     */
    FlowContext(int image_count, int buffer_count);
 
    /**
     * Create FlowContext. All images and buffers are assumed to just have been created.
     * @param images all images to be used by flow sections
     * @param buffers all buffers to be used by flow sections
     */
    FlowContext(const vector_ext<ExtImage>& images, const vector_ext<Buffer>& buffers);
   
    /**
     * Create FlowContext with given images, buffers and their respective states
     * @param images all images to be used by flow sections
     * @param image_states list of states, one for each image
     * @param buffers all buffers to be used by flow sections
     * @param buffer_states list of states, one for each buffer
     */
    FlowContext(const vector_ext<ExtImage>& images, const vector<PipelineImageState>& image_states, const vector_ext<Buffer>& buffers, const vector<PipelineBufferState>& buffer_states);
    
    void addImage(int i, ExtImage&& image);
    void addImage(int i, const ExtImage& image);
    void addBuffer(int i, Buffer&& buffer);
    void addBuffer(int i, const Buffer& buffer);
    void addBufferVertex(int i, const Buffer& buffer, uint32_t vertex_count);
    void addBufferVertexUploadable(int i, const Buffer& buffer, uint32_t vertex_count);
    void addBufferUploadable(int i, const Buffer& buffer);
    void addImageUploadable(int i, const ExtImage& image);

    //get the required size of a staging buffer to copy all requested data
    VkDeviceSize getLargestCopySize() const;

    //assign device local memory to all images and buffers
    void allocate();

    /**
     * Get a reference to the image with given index
     * @param index the index of image to return
     */
    ExtImage& getImage(uint32_t index);

    /**
     * Get a const reference to the image with given index
     * @param index the index of image to return
     */
    const ExtImage& getImage(uint32_t index) const;

    /**
     * Get a reference to the buffer with given index
     * @param index the index of buffer to return
     */
    Buffer& getBuffer(uint32_t index);

    /**
     * Get a const reference to the buffer with given index
     * @param index the index of buffer to return
     */
    const Buffer& getBuffer(uint32_t index) const;

    /**
     * Get a reference to the state of image of given index.
     * @param index the index of image, the state of which to get
     */
    PipelineImageState& getImageState(uint32_t index);

    /**
     * Get a const reference to the state of image of given index.
     * @param index the index of image, the state of which to get
     */
    const PipelineImageState& getImageState(uint32_t index) const;

    /**
     * Get a reference to the state of buffer of given index.
     * @param index the index of buffer, the state of which to get
     */
    PipelineBufferState& getBufferState(uint32_t index);

    /**
     * Get a const reference to the state of buffer of given index.
     * @param index the index of buffer, the state of which to get
     */
    const PipelineBufferState& getBufferState(uint32_t index) const;

    /**
     * @brief Save a pointer to given push constant data
     * 
     * @param section_name the name, using which the data can be later accessed.
     * @param data the push constant data to save
     */
    void registerPushConstants(const string& section_name, PushConstantData& data);

    /**
     * @brief Get push constants data based on the name given
     * 
     * @param section_name the name under which push constants were registered
     */
    PushConstantData& getPushConstants(const string& section_name);

    /**
     * @brief Save how many vertices a vertex buffer holds
     * 
     * @param descriptor_index index of the vertex buffer, can be used later to retrieve the count
     * @param vertex_count how many vertices the buffer contains
     */
    void registerVertexBuffer(uint32_t descriptor_index, uint32_t vertex_count);

    /**
     * @brief Get how many vertices a vertex buffer with given index contains
     * 
     * @param descriptor_index index of the vertex buffer
     */
    uint32_t getVertexCount(uint32_t descriptor_index);
private:
    //Ensure image at index i hasn't been written before (Image.isValid() must be false to pass the test)
    void ensureNotWrittenImg(int i) const;

    //Ensure buffer at index i hasn't been written before (Buffer.isValid() must be false to pass the test)
    void ensureNotWrittenBuf(int i) const;

    //max_copy_size = max(new_size, max_copy_size)
    void updateLargestCopySize(VkDeviceSize new_size);
};



/**
 * FlowSectionDescriptorUsage
 *  - Records descriptor usage during a section
 */
class FlowSectionDescriptorUsage{
public:
    //true if descriptor is an image, false if is a buffer
    bool is_image;
    //index of descriptor in associated descriptor context
    int descriptor_index;
    //stages during which the descriptor will be used
    VkPipelineStageFlags usage_stages;
    //descriptor state, different for image and buffer
    union{
        ImageState image;
        BufferState buffer;
    } state;
    
    /**
     * Constructs FlowSectionDescriptorUsage for an image
     * @param descriptor_index image index in associated descriptor context
     * @param usage_stages the stages, during which the image will be used
     * @param img_state the state image is required to be in when used
     */
    FlowSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state);
    
    /**
     * Constructs FlowSectionDescriptorUsage for a buffer
     * @param descriptor_index buffer index in associated descriptor context
     * @param usage_stages the stages, during which the buffer will be used
     * @param buf_state the state buffer is required to be in when used
     */
    FlowSectionDescriptorUsage(int descriptor_index_, VkPipelineStageFlags usage_stages, BufferState buf_state);

    /**
     * Convert FlowSectionDescriptorUsage to PipelineImageState - copy stage flags and image state
     */
    PipelineImageState toImageState() const;

    /**
     * Convert FlowSectionDescriptorUsage to PipelineBufferState - copy stage flags and buffer state
     */
    PipelineBufferState toBufferState() const;

    /**
     * Returns is_image
     */
    bool isImage() const;
};



/**
 * FlowPipelineSectionDescriptorUsage
 *  - Holds all information for one descriptor usage and data to update it
 */
class FlowPipelineSectionDescriptorUsage{
protected:
    //holds data about descriptor usage in flow section
    FlowSectionDescriptorUsage usage;
    //holds info about how to update the descriptor
    DescriptorUpdateInfo info;
public:
    /**
     * Construct a FlowPipelineSectionDescriptorUsage for a buffer with given paramateres
     * @param descriptor_index index of this buffer in flow descriptor context
     * @param usage_stages the stages during which the buffer will be used
     * @param buf_state the state the buffer should be in
     * @param desc_info the info required to update this buffer
     */
    FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state, const DescriptorUpdateInfo& desc_info);

    /**
     * Construct a FlowPipelineSectionDescriptorUsage for an image with given paramateres
     * @param descriptor_index index of this image in flow descriptor context
     * @param usage_stages the stages during which the image will be used
     * @param img_state the state the image should be in
     * @param desc_info the info required to update this image
     */
    FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state, const DescriptorUpdateInfo& desc_info);

    /**
     * Get a const reference to descriptor usage in current flow section
     */
    const FlowSectionDescriptorUsage& getUsage() const;

    /**
     * Use given context to fill in missing pieces of update info, then return it
     * @param ctx the flow descriptor context associated with this descriptor
     */
    DescriptorUpdateInfo getUpdateInfo(FlowContext& ctx) const;
};



#endif