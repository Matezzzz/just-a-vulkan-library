#include "flow_section_descriptors.h"

FlowStorageImageR::FlowStorageImageR(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, ImgState::StorageR, StorageImageUpdateInfo(name, VK_NULL_HANDLE, ImgState::StorageR.layout))
{}


FlowStorageImageW::FlowStorageImageW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, ImgState::StorageW, StorageImageUpdateInfo(name, VK_NULL_HANDLE, ImgState::StorageW.layout))
{}


FlowStorageImageRW::FlowStorageImageRW(const string& name, int descriptor_index, VkPipelineStageFlags usage_stages) :
    FlowPipelineSectionDescriptorUsage(descriptor_index, usage_stages, ImgState::StorageRW, StorageImageUpdateInfo(name, VK_NULL_HANDLE, ImgState::StorageRW.layout))
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