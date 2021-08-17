#ifndef FLOW_SECTIONS_BASE_H
#define FLOW_SECTIONS_BASE_H

#include "../00_base/vulkan_base.h"
#include "../04_memory_objects/buffer.h"
#include "../04_memory_objects/image.h"
#include "../07_shaders/pipelines_context.h"
#include <glm/glm.hpp>


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
    
    /**
     * Construct PipelineImageState with no last use, no last layout and access
     */
    PipelineImageState();

    /**
     * Construct PipelineImageState with given state and last use
     * @param state the state the image is currently in
     * @param last_use the last flags with which it was used in a pipeline
     */
    PipelineImageState(ImageState state, VkPipelineStageFlags last_use);
};



/**
 * FlowDescriptorContext
 *  - Holds all images and buffers to be used by flow sections, + their current states
 */
class FlowDescriptorContext{
    //vector of all images used by flow sections
    vector<ExtImage> m_images;
    //states, one for each image
    vector<PipelineImageState> m_image_states;

    //vector of all buffers used by flow sections
    vector<Buffer> m_buffers;
    //states, one for each buffer
    vector<PipelineBufferState> m_buffer_states;
public:
    /**
     * Create FlowDescriptorContext. All images and buffers are assumed to just have been created.
     * @param images all images to be used by flow sections
     * @param buffers all buffers to be used by flow sections
     */
    FlowDescriptorContext(const vector<ExtImage>& images, const vector<Buffer>& buffers);
    /**
     * Create FlowDescriptorContext with given images, buffers and their respective states
     * @param images all images to be used by flow sections
     * @param image_states list of states, one for each image
     * @param buffers all buffers to be used by flow sections
     * @param buffer_states list of states, one for each buffer
     */
    FlowDescriptorContext(const vector<ExtImage>& images, const vector<PipelineImageState>& image_states, const vector<Buffer>& buffers, const vector<PipelineBufferState>& buffer_states);
    
    /**
     * Get a reference to the image with given index
     * @param index the index of image to return
     */
    ExtImage& getImage(uint32_t index);

    /**
     * Get a reference to the buffer with given index
     * @param index the index of buffer to return
     */
    Buffer& getBuffer(uint32_t index);

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
    DescriptorUpdateInfo getUpdateInfo(FlowDescriptorContext& ctx) const;
};



/**
 * FlowStorageImage
 *  - Constructs FlowPipelineSectionDescriptorUsage for storage images
 */
class FlowStorageImage : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct FlowStorageImage from given info
     * @param name name of image in shaders
     * @param descriptor_index index of image in associated flow descriptor context
     * @param usage_stages the stages during which image will be used
     * @param img_state the state image will be in when used
     */
    FlowStorageImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state);
};


/**
 * FlowCombinedImage
 *  - Constructs FlowPipelineSectionDescriptorUsage for combined image samplers
 */
class FlowCombinedImage : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct FlowCombinedImage from given info
     * @param name name of image in shaders
     * @param descriptor_index index of image in associated flow descriptor context
     * @param usage_stages the stages during which image will be used
     * @param img_state the state image will be in when used
     * @param sampler the sampler to use alongside image
     */
    FlowCombinedImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state, VkSampler sampler);
};


/**
 * FlowUniformBuffer
 *  - Constructs FlowPipelineSectionDescriptorUsage for uniform buffers
 */
class FlowUniformBuffer : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct FlowUniformBuffer from given info
     * @param name name of buffer in shaders
     * @param descriptor_index index of buffer in associated flow descriptor context
     * @param usage_stages the stages during which buffer will be used
     * @param buf_state the state buffer will be in when used
     */
    FlowUniformBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state);
};

/**
 * FlowStorageBuffer
 *  - Constructs FlowPipelineSectionDescriptorUsage for storage buffers
 */
class FlowStorageBuffer : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct FlowStorageBuffer from given info
     * @param name name of buffer in shaders
     * @param descriptor_index index of buffer in associated flow descriptor context
     * @param usage_stages the stages during which buffer will be used
     * @param buf_state the state buffer will be in when used
     */
    FlowStorageBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state);
};



#endif