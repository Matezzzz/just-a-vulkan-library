#include "flow_sections_base.h"


Size3::Size3(const glm::uvec3& v) : glm::uvec3(v)
{}
uint32_t Size3::volume() const{
    return x * y * z;
}






PipelineBufferState::PipelineBufferState(BufferState state, VkPipelineStageFlags last_use_) : BufferState(state), last_use(last_use_)
{}

PipelineBufferState::PipelineBufferState() : last_use(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
{}






PipelineImageState::PipelineImageState() : last_use(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
{}
PipelineImageState::PipelineImageState(ImageState state, VkPipelineStageFlags last_use_ = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT) :
    ImageState(state), last_use(last_use_)
{}








//constructor for images
FlowSectionDescriptorUsage::FlowSectionDescriptorUsage(int descriptor_index_, VkPipelineStageFlags usage_stages_, ImageState img_state) : 
    is_image(true), descriptor_index(descriptor_index_), usage_stages(usage_stages_), state{.image=img_state}
{}
//constructor for buffers
FlowSectionDescriptorUsage::FlowSectionDescriptorUsage(int descriptor_index_, VkPipelineStageFlags usage_stages_, BufferState buf_state) : 
    is_image(false), descriptor_index(descriptor_index_), usage_stages(usage_stages_), state{.buffer=buf_state}
{}
PipelineImageState FlowSectionDescriptorUsage::toImageState() const{
    return PipelineImageState{state.image, usage_stages};
}
PipelineBufferState FlowSectionDescriptorUsage::toBufferState() const{
    return PipelineBufferState{state.buffer, usage_stages};
}
bool FlowSectionDescriptorUsage::isImage() const{
    return is_image;
}



FlowDescriptorContext::FlowDescriptorContext(){}
FlowDescriptorContext::FlowDescriptorContext(const vector<ExtImage>& images, const vector<Buffer>& buffers) :
    FlowDescriptorContext(images, vector<PipelineImageState>(images.size()), buffers, vector<PipelineBufferState>(buffers.size()))
{}
FlowDescriptorContext::FlowDescriptorContext(const vector<ExtImage>& images, const vector<PipelineImageState>& image_states, const vector<Buffer>& buffers, const vector<PipelineBufferState>& buffer_states) : 
    m_images(images), m_image_states(image_states), m_buffers(buffers), m_buffer_states(buffer_states)
{}
ExtImage& FlowDescriptorContext::getImage(uint32_t index){return m_images[index];}
Buffer& FlowDescriptorContext::getBuffer(uint32_t index) {return m_buffers[index];}
PipelineImageState& FlowDescriptorContext::getImageState(uint32_t index){return m_image_states[index];}
const PipelineImageState& FlowDescriptorContext::getImageState(uint32_t index) const{return m_image_states[index];}
PipelineBufferState& FlowDescriptorContext::getBufferState(uint32_t index){return m_buffer_states[index];}
const PipelineBufferState& FlowDescriptorContext::getBufferState(uint32_t index) const{return m_buffer_states[index];}





FlowPipelineSectionDescriptorUsage::FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state, const DescriptorUpdateInfo& desc_info) : 
    usage(descriptor_index, usage_stages, buf_state), info{desc_info}
{}
FlowPipelineSectionDescriptorUsage::FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state, const DescriptorUpdateInfo& desc_info) : 
    usage(descriptor_index, usage_stages, img_state), info{desc_info}
{}
const FlowSectionDescriptorUsage& FlowPipelineSectionDescriptorUsage::getUsage() const{
    return usage;
}
DescriptorUpdateInfo FlowPipelineSectionDescriptorUsage::getUpdateInfo(FlowDescriptorContext& ctx) const{
    DescriptorUpdateInfo i2(info);
    //if descriptor is an image, get its' image info and write a reference to image from given context
    if (i2.isImage()){
        i2.imageInfo()->imageView = ctx.getImage(usage.descriptor_index);
    //if descriptor is a buffer, get buffer info instead and write a reference to buffer from context
    }else{
        i2.bufferInfo()->buffer = ctx.getBuffer(usage.descriptor_index);
    }
    //return buffer info with updated handle to image/buffer
    return i2;
}



FlowStorageImage::FlowStorageImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, img_state, StorageImageUpdateInfo(name, VK_NULL_HANDLE, img_state.layout))
{}



FlowCombinedImage::FlowCombinedImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state, VkSampler sampler) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, img_state, CombinedImageSamplerUpdateInfo{name, VK_NULL_HANDLE, sampler, img_state.layout})
{}



FlowUniformBuffer::FlowUniformBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, buf_state, UniformBufferUpdateInfo{name, VK_NULL_HANDLE})
{}



FlowStorageBuffer::FlowStorageBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, buf_state, StorageBufferUpdateInfo{name, VK_NULL_HANDLE})
{}
