#include "flow_section_descriptors.h"

FlowStorageImage::FlowStorageImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, ImageState img_state) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, img_state, StorageImageUpdateInfo(name, VK_NULL_HANDLE, img_state.layout))
{}


FlowCombinedImage::FlowCombinedImage(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages, VkSampler sampler) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, ImgState::Sampled, CombinedImageSamplerUpdateInfo{name, VK_NULL_HANDLE, sampler, ImgState::Sampled.layout})
{}


FlowUniformBuffer::FlowUniformBuffer(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, BufState::Uniform, UniformBufferUpdateInfo{name, VK_NULL_HANDLE})
{}


FlowStorageBufferR::FlowStorageBufferR(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, BufState::StorageR, StorageBufferUpdateInfo{name, VK_NULL_HANDLE})
{}


FlowStorageBufferW::FlowStorageBufferW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, BufState::StorageW, StorageBufferUpdateInfo{name, VK_NULL_HANDLE})
{}


FlowStorageBufferRW::FlowStorageBufferRW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, BufState::StorageRW, StorageBufferUpdateInfo{name, VK_NULL_HANDLE})
{}