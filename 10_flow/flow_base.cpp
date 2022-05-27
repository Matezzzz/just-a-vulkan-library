#include "flow_base.h"


Size3::Size3(const glm::uvec3& v) : glm::uvec3(v)
{}
uint32_t Size3::volume() const{
    return x * y * z;
}






PipelineBufferState::PipelineBufferState(BufferState state, VkPipelineStageFlags last_use_) : BufferState(state), last_use(last_use_)
{}

PipelineBufferState::PipelineBufferState() : last_use(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
{}






PipelineImageState::PipelineImageState() : last_use(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT), barrier_done(false)
{}
PipelineImageState::PipelineImageState(ImageState state, VkPipelineStageFlags last_use_, bool barrier_done_) :
    ImageState(state), last_use(last_use_), barrier_done(barrier_done_)
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



FlowContext::FlowContext(int image_count, int buffer_count) :
    FlowContext(vector_ext<ExtImage>(image_count), vector_ext<Buffer>(buffer_count))
{}
FlowContext::FlowContext(const vector_ext<ExtImage>& images, const vector_ext<Buffer>& buffers) :
    FlowContext(images, vector<PipelineImageState>(images.size()), buffers, vector<PipelineBufferState>(buffers.size()))
{}
FlowContext::FlowContext(const vector_ext<ExtImage>& images, const vector<PipelineImageState>& image_states, const vector_ext<Buffer>& buffers, const vector<PipelineBufferState>& buffer_states) : 
    m_images(images), m_image_states(image_states), m_buffers(buffers), m_buffer_states(buffer_states)
{}
void FlowContext::addImage(int i, ExtImage&& image){
    ensureNotWrittenImg(i); m_images[i] = image;
}
void FlowContext::addImage(int i, const ExtImage& image){
    ensureNotWrittenImg(i); m_images[i] = image;
}
void FlowContext::addBuffer(int i, Buffer&& buffer){
    ensureNotWrittenBuf(i); m_buffers[i] = buffer;
}
void FlowContext::addBuffer(int i, const Buffer& buffer){
    ensureNotWrittenBuf(i); m_buffers[i] = buffer;
}
void FlowContext::addBufferVertex(int i, const Buffer& buffer, uint32_t vertex_count){
    registerVertexBuffer(i, vertex_count);
    addBuffer(i, buffer);
}
void FlowContext::addBufferVertexUploadable(int i, const Buffer& buffer, uint32_t vertex_count){
    updateLargestCopySize(buffer.getSize());
    addBufferVertex(i, buffer, vertex_count);
}
void FlowContext::addBufferUploadable(int i, const Buffer& buffer){
    updateLargestCopySize(buffer.getSize());
    addBuffer(i, buffer);
}
void FlowContext::addImageUploadable(int i, const ExtImage& image){
    updateLargestCopySize(image.getSizeInBytes());
    addImage(i, image);
}
VkDeviceSize FlowContext::getLargestCopySize() const{
    return m_max_copy_size;
}
void FlowContext::allocate(){
    for (uint32_t i = 0; i < m_images.size(); i++){
        DEBUG_CHECK("Image hasn't been created" << i, !m_images[i].isValid())
    }
    for (uint32_t i = 0; i < m_buffers.size(); i++){
        DEBUG_CHECK("Buffer hasn't been created" << i, !m_buffers[i].isValid())
    }
    ImageMemoryObject image_mem{m_images, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
    BufferMemoryObject buffer_mem{m_buffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};
}
ExtImage& FlowContext::getImage(uint32_t index){return m_images[index];}
const ExtImage& FlowContext::getImage(uint32_t index) const {return m_images[index];}
Buffer& FlowContext::getBuffer(uint32_t index) {return m_buffers[index];}
const Buffer& FlowContext::getBuffer(uint32_t index) const {return m_buffers[index];}
PipelineImageState& FlowContext::getImageState(uint32_t index){return m_image_states[index];}
const PipelineImageState& FlowContext::getImageState(uint32_t index) const{return m_image_states[index];}
PipelineBufferState& FlowContext::getBufferState(uint32_t index){return m_buffer_states[index];}
const PipelineBufferState& FlowContext::getBufferState(uint32_t index) const{return m_buffer_states[index];}
void FlowContext::registerPushConstants(const string& section_name, PushConstantData& data){
    DEBUG_CHECK ("It is not allowed to have two push constant sections of the same name", m_push_constants.contains(section_name))
    m_push_constants.emplace(section_name, &data);
}
PushConstantData& FlowContext::getPushConstants(const string& section_name){
    return *m_push_constants[section_name];
}
void FlowContext::registerVertexBuffer(uint32_t descriptor_index, uint32_t vertex_count){
    DEBUG_CHECK ("Vertex buffer registered for a second time", m_vertex_buffer_sizes.contains(descriptor_index))
    m_vertex_buffer_sizes.emplace(descriptor_index, vertex_count);
}
uint32_t FlowContext::getVertexCount(uint32_t descriptor_index){
    return m_vertex_buffer_sizes[descriptor_index];
}
void FlowContext::ensureNotWrittenImg(int i) const{
    DEBUG_CHECK("Adding an image that was already added before: " << i, m_images[i].isValid())
}
void FlowContext::ensureNotWrittenBuf(int i) const{
    DEBUG_CHECK("Adding a buffer that was already added before: " << i, m_buffers[i].isValid())
}
void FlowContext::updateLargestCopySize(VkDeviceSize new_size){
    m_max_copy_size = std::max(new_size, m_max_copy_size);
}






FlowPipelineSectionDescriptorUsage::FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, BufferState buf_state, const DescriptorUpdateInfo& desc_info) : 
    usage(descriptor_index, usage_stages, buf_state), info{desc_info}
{}
FlowPipelineSectionDescriptorUsage::FlowPipelineSectionDescriptorUsage(int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state, const DescriptorUpdateInfo& desc_info) : 
    usage(descriptor_index, usage_stages, img_state), info{desc_info}
{}
const FlowSectionDescriptorUsage& FlowPipelineSectionDescriptorUsage::getUsage() const{
    return usage;
}
DescriptorUpdateInfo FlowPipelineSectionDescriptorUsage::getUpdateInfo(FlowContext& ctx) const{
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
