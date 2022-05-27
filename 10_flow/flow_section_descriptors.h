#ifndef FLOW_SECTION_DESCRIPTORS
#define FLOW_SECTION_DESCRIPTORS

#include "../10_flow/flow_base.h"



/**
 * FlowStorageImageR
 *  - Constructs FlowPipelineSectionDescriptorUsage for read only storage images
 */
class FlowStorageImageR : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct a read only FlowStorageImage from given info
     * @param name name of image in shaders
     * @param descriptor_index index of image in associated flow descriptor context
     * @param usage_stages the stages during which image will be used
     */
    FlowStorageImageR(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};


/**
 * FlowStorageImage
 *  - Constructs FlowPipelineSectionDescriptorUsage for write only storage images
 */
class FlowStorageImageW : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct a write only FlowStorageImage from given info
     * @param name name of image in shaders
     * @param descriptor_index index of image in associated flow descriptor context
     * @param usage_stages the stages during which image will be used
     */
    FlowStorageImageW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};

/**
 * FlowStorageImage
 *  - Constructs FlowPipelineSectionDescriptorUsage for storage images allowing reads and writes
 */
class FlowStorageImageRW : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct FlowStorageImage allowing reads and writes from given info
     * @param name name of image in shaders
     * @param descriptor_index index of image in associated flow descriptor context
     * @param usage_stages the stages during which image will be used
     */
    FlowStorageImageRW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
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
    FlowCombinedImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, VkSampler sampler);
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
    FlowUniformBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};


/**
 * FlowStorageBufferR
 *  - Constructs FlowPipelineSectionDescriptorUsage for read only storage buffers
 */
class FlowStorageBufferR : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct read only FlowStorageBuffer from given info
     * @param name name of buffer in shaders
     * @param descriptor_index index of buffer in associated flow descriptor context
     * @param usage_stages the stages during which buffer will be used
     */
    FlowStorageBufferR(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};


/**
 * FlowStorageBufferW
 *  - Constructs FlowPipelineSectionDescriptorUsage for write only storage buffers
 */
class FlowStorageBufferW : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct write only FlowStorageBuffer from given info
     * @param name name of buffer in shaders
     * @param descriptor_index index of buffer in associated flow descriptor context
     * @param usage_stages the stages during which buffer will be used
     */
    FlowStorageBufferW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};


/**
 * FlowStorageBufferRW
 *  - Constructs FlowPipelineSectionDescriptorUsage for storage buffers allowing both reads and writes
 */
class FlowStorageBufferRW : public FlowPipelineSectionDescriptorUsage{
public:
    /**
     * Construct RW FlowStorageBuffer from given info
     * @param name name of buffer in shaders
     * @param descriptor_index index of buffer in associated flow descriptor context
     * @param usage_stages the stages during which buffer will be used
     */
    FlowStorageBufferRW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages);
};



#endif