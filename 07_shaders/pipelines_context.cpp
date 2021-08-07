#include "pipelines_context.h"
#include "../01_device/device.h"
#include "../01_device/allocator.h"
#include "../08_pipeline/pipeline.h"


DescriptorUpdateInfo::DescriptorUpdateInfo(const string& name, VkDescriptorType type, VkImageLayout layout, VkImageView image, VkSampler sampler) :
    m_name{name}, m_type(UPDATE_INFO_IMAGE), m_info{.image=new VkDescriptorImageInfo{sampler, image, layout}}, m_vulkan_type(type)
{}
DescriptorUpdateInfo::DescriptorUpdateInfo(const string& name, VkDescriptorType type, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) :
    m_name{name}, m_type(UPDATE_INFO_BUFFER), m_info{.buffer=new VkDescriptorBufferInfo{buffer, offset, range}}, m_vulkan_type(type)
{}
DescriptorUpdateInfo::DescriptorUpdateInfo(const DescriptorUpdateInfo& i) :
    m_name{i.m_name}, m_type{i.m_type}, m_info{.copied=i.copyInfo()}, m_vulkan_type{i.m_vulkan_type}
{}
DescriptorUpdateInfo& DescriptorUpdateInfo::operator=(const DescriptorUpdateInfo& i){
    m_name = i.m_name;
    m_type = i.m_type;
    m_info.copied = i.copyInfo();
    m_vulkan_type = i.m_vulkan_type;
    return *this;
}
DescriptorUpdateInfo::~DescriptorUpdateInfo(){
    //delete info based on type
    if (isImage()){
        delete imageInfo();
    }else{
        delete bufferInfo();
    }
}
bool DescriptorUpdateInfo::isImage() const{
    return (m_type == UPDATE_INFO_IMAGE);
}
const string& DescriptorUpdateInfo::getName() const{
    return m_name;
}
VkDescriptorType DescriptorUpdateInfo::getType() const{
    return m_vulkan_type;
}
VkDescriptorImageInfo* DescriptorUpdateInfo::imageInfo(){
    //return ptr if updating image, otherwise nullptr
    if (isImage()) return m_info.image;
    return nullptr;
}
const VkDescriptorImageInfo* DescriptorUpdateInfo::imageInfo() const{
    //return ptr if updating image, otherwise nullptr
    if (isImage()) return m_info.image;
    return nullptr;
}
VkDescriptorBufferInfo* DescriptorUpdateInfo::bufferInfo(){
    //return ptr if updating buffer, otherwise nullptr
    if (!isImage()) return m_info.buffer;
    return nullptr;
}
const VkDescriptorBufferInfo* DescriptorUpdateInfo::bufferInfo() const{
    //return ptr if updating buffer, otherwise nullptr
    if (!isImage()) return m_info.buffer;
    return nullptr;
}
void* DescriptorUpdateInfo::copyInfo() const{
    if (m_type == UPDATE_INFO_IMAGE){
        return (void*) new VkDescriptorImageInfo(*m_info.image);
    }
    return (void*) new VkDescriptorBufferInfo(*m_info.buffer);
}



SamplerUpdateInfo::SamplerUpdateInfo(const string& name, VkSampler sampler) :
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_SAMPLER, VK_IMAGE_LAYOUT_UNDEFINED, VK_NULL_HANDLE, sampler)
{}

CombinedImageSamplerUpdateInfo::CombinedImageSamplerUpdateInfo(const string& name, VkImageView image, VkSampler sampler, VkImageLayout layout) :
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, layout, image, sampler)
{}

SampledImageUpdateInfo::SampledImageUpdateInfo(const string& name, VkImageView image, VkImageLayout layout) :
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, layout, image, VK_NULL_HANDLE)
{}

StorageImageUpdateInfo::StorageImageUpdateInfo(const string& name, VkImageView image, VkImageLayout layout) : 
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, layout, image, VK_NULL_HANDLE)
{}

UniformBufferUpdateInfo::UniformBufferUpdateInfo(const string& name, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) :
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, buffer, offset, range)
{}

StorageBufferUpdateInfo::StorageBufferUpdateInfo(const string& name, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) :
    DescriptorUpdateInfo(name, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, buffer, offset, range)
{}



ShaderStages::ShaderStages(const vector<ShaderData>& shaders){
    //reserve space for stages
    reserve(shaders.size());
    //add one PipelineShaderStageInfo for each shader stage
    for (const ShaderData& shader : shaders){
        push_back(PipelineShaderStageInfo(shader.compiled_data, shader.shader_stage));
    }
}
const VkPipelineShaderStageCreateInfo& ShaderStages::getCompute() const{
    //if there are more than one stage, shader stages cannot be compute, print error
    if (size() != 1) PRINT_ERROR("Cannot retrieve the right amount of stages(1), size: " << size())
    //if the only stage isn't compute, print error
    if ((*this)[0].stage != VK_SHADER_STAGE_COMPUTE_BIT) PRINT_ERROR("Trying to retrieve compute shader stage, but the first stage isn't compute")
    //return first and only stage
    return (*this)[0];
}




DescriptorSet::DescriptorSet() : m_set{VK_NULL_HANDLE}
{}
DescriptorSet::DescriptorSet(VkDescriptorSet set, const ShaderDataDescriptorSet& layout) : m_set(set), m_layout(&layout)
{}
DescriptorSet::operator VkDescriptorSet(){
    return m_set;
}


void DescriptorSetCounter::addDescriptors(const DescriptorData& data, uint32_t descriptor_count){
    //if descriptor set is valid
    if (data.exists()){
        //register descriptors of given type if it's valid
        if (data.getType() >= 0 && data.getType() < 11){
            m_descriptor_counts[(int32_t) data.getType()] += descriptor_count;
        }
    }
}
vector<VkDescriptorPoolSize> DescriptorSetCounter::getPoolSizes() const{
    //convert internal counts objects to vector of VkDescriptorPoolSize
    vector<VkDescriptorPoolSize> sizes;
    for (uint32_t i = 0; i < 11; i++){
        //if there are any descriptors of type, add them to the vector
        if (m_descriptor_counts[i]){
            sizes.push_back(VkDescriptorPoolSize{(VkDescriptorType) i, m_descriptor_counts[i]});
        }
    }
    return sizes;
}



void DescriptorSetManager::reserveSets(const ShaderDataDescriptorSet& set, uint32_t set_count){
    //if set is shared, it will be reserved somewhere else, return
    if (set.isShared()) return;
    //update the total set amount by given count
    m_set_count += set_count;
    //go through all descriptors in set and update total descriptor counts
    for (const DescriptorData& d : set){
        m_set_counter.addDescriptors(d, set_count);
    }
}
VkDescriptorSet DescriptorSetManager::allocateSet(VkDescriptorSetLayout layout){
    //allocate given set
    return m_descriptor_pool.allocateSet({layout});
}
void DescriptorSetManager::createPool(){
    //convert descriptor counts to vector<VkDescriptorPoolSize>
    vector<VkDescriptorPoolSize> descriptor_counts{m_set_counter.getPoolSizes()};
    //fill create info structure
    VkDescriptorPoolCreateInfo info{
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, nullptr, 0,
        m_set_count, (uint32_t) descriptor_counts.size(), descriptor_counts.data()
    };
    //create descriptor pool
    m_descriptor_pool = DescriptorPool(g_allocator.get().createDescriptorPool(info));
}




template<typename ...Ts>
SetVector::SetVector(Ts&... sets){
    //reserve space for all sets
    reserve(sizeof...(sets));
    //set pointers to sets
    initFunc(sets...);
}




PushConstantData::PushConstantData(const PushConstantLayout& layout) : UniformBufferLayoutData(layout), m_stages(layout.getAllStages())
{}
VkShaderStageFlags PushConstantData::getStages() const {
    return m_stages;
}
uint32_t PushConstantData::size() const{
    return roundUpToMemoryBlock(UniformBufferLayoutData::size(), 4ULL);
}



PipelineContext::PipelineContext(const ShaderDirectoryData& shader_dir_data, DescriptorSetManager& descriptor_set_manager) : 
    m_shader_stages(shader_dir_data.shaders), m_pipeline_layout(VK_NULL_HANDLE), m_set_manager(descriptor_set_manager)
{
    //go through all shaders
    for (const ShaderData& data : shader_dir_data.shaders){
        //load all descriptors, inputs, outputs and push constants from current shader stage
        ShaderDataInfo current_shader_data(data.base_data, data.shader_stage);
        //combine all found descriptors with ones found in previous stages
        m_shader_info.combineWithShaderData(current_shader_data);
    }
}
void PipelineContext::makeSharedDescriptorSet(uint32_t set_i_1, PipelineContext& ctx, uint32_t set_i_2){
    m_shader_info.makeShared(set_i_1, ctx.getDescriptorSet(set_i_2));
}
void PipelineContext::makeSharedPushConstantLayout(PipelineContext&){
    PRINT_WARN("Shared push constants not implemented yet");
}
PushConstantData PipelineContext::createPushConstantData(){
    return PushConstantData(m_shader_info.getPushConstantLayout());
}
UniformBufferData PipelineContext::createUniformBufferData(uint32_t set_i, const string& descriptor_name){
    return m_shader_info.getSets()[set_i].createUniformBufferData(descriptor_name);
}
Pipeline PipelineContext::createPipeline(const PipelineInfo& info, VkRenderPass render_pass, uint32_t subpass_index){
    //ensure m_pipeline_layout was created already
    keepOrCreatePipelineLayout();
    //create pipeline and return it
    return Pipeline{info.create(m_shader_stages, m_pipeline_layout, render_pass, subpass_index), m_pipeline_layout, VK_PIPELINE_BIND_POINT_GRAPHICS};
}
Pipeline PipelineContext::createComputePipeline(){
    //ensure m_pipeline_layout was created already
    keepOrCreatePipelineLayout();
    //create compute pipeline and return it
    return Pipeline(ComputePipelineInfo{}.create(m_shader_stages.getCompute(), m_pipeline_layout), m_pipeline_layout, VK_PIPELINE_BIND_POINT_COMPUTE);
}
ShaderDataDescriptorSet& PipelineContext::getDescriptorSet(uint32_t i){
    return m_shader_info.getSets()[i];
}
void PipelineContext::keepOrCreatePipelineLayout(){
    //if layout was created already, return
    if (m_pipeline_layout) return;

    //create pipeline layout info
    PipelineLayoutInfo pipeline_layout_info;
    for (ShaderDataDescriptorSet& s : m_shader_info.getSets()){
        //fill pipeline layout info with descriptor set layouts
        pipeline_layout_info.addDescriptorSetLayout(s.getOrCreateLayout());
    }
    //set push constant ranges
    pipeline_layout_info.setPushConstants(m_shader_info.getPushConstantLayout().getPushConstantRanges());
    //create pipeline layout
    m_pipeline_layout = pipeline_layout_info.create();
}








DirectoryPipelinesContext::DirectoryPipelinesContext(const string& directory){
    //search a directory for shader folders
    ShaderDirectoryTree shader_tree(directory);
    //create a pipeline context for each shader folder
    for (const ShaderDirectoryData& dir_data : shader_tree.directories){
        m_pipeline_contexts.insert({dir_data.dir_name, PipelineContext(dir_data, m_descriptor_set_manager)});
    }
}
PipelineContext& DirectoryPipelinesContext::getContext(const string& name){
    //find context of given name
    auto itr = m_pipeline_contexts.find(name);
    //if none could be found, print error
    if (itr == m_pipeline_contexts.end()){
        PRINT_ERROR("Pipeline context of name " << name << " could not be found.")
    }
    //return reference to given pipeline context
    return itr->second;
}
void DirectoryPipelinesContext::createDescriptorPool(){
    m_descriptor_set_manager.createPool();
}

 