#include "sampler.h"

//parameters of default sampler are described in header
SamplerInfo::SamplerInfo(): m_info{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO, nullptr, 0,
    VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_MIPMAP_MODE_NEAREST,
    VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT,
    0.f, false, 0.f,
    false, VK_COMPARE_OP_NEVER, 
    0.f, 0.f, VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK, false}
{}


SamplerInfo& SamplerInfo::setFilters(VkFilter min_filter, VkFilter mag_filter){
    m_info.minFilter = min_filter;
    m_info.magFilter = mag_filter;
    return *this;
}
SamplerInfo& SamplerInfo::setMipmapFilterMode(VkSamplerMipmapMode mipmap_mode){
    m_info.mipmapMode = mipmap_mode;
    return *this;
}
SamplerInfo& SamplerInfo::setWrapMode(VkSamplerAddressMode wrap_mode){
    m_info.addressModeU = wrap_mode;
    m_info.addressModeV = wrap_mode;
    m_info.addressModeW = wrap_mode;
    return *this;  
}
SamplerInfo& SamplerInfo::setWrapModeX(VkSamplerAddressMode wrap_mode){
    m_info.addressModeU = wrap_mode;
    return *this;
}
SamplerInfo& SamplerInfo::setWrapModeY(VkSamplerAddressMode wrap_mode){
    m_info.addressModeV = wrap_mode;
    return *this;
}
SamplerInfo& SamplerInfo::setWrapModeZ(VkSamplerAddressMode wrap_mode){
    m_info.addressModeW = wrap_mode;
    return *this;
}
/* THE FOLLOWING MAY BE REQUIRED TO BE IMPLEMENTED IN THE FUTURE. CURRENTLY THEY ARE NOT NEEDED.
SamplerInfo& setMipmapBias(float bias);
SamplerInfo& enableAnisotropy();
SamplerInfo& disableAnisotropy();
SamplerInfo& enableCompare(VkCompareOp compare_operation);
SamplerInfo& disableCompare();
SamplerInfo& setLODRange(float min_lod, float max_lod);
*/
SamplerInfo& SamplerInfo::setBorderColor(VkBorderColor color){
    m_info.borderColor = color;
    return *this;
}
SamplerInfo& SamplerInfo::enableNormalizedCoordinates(){
    m_info.unnormalizedCoordinates = false;
    return *this;
}
SamplerInfo& SamplerInfo::disableNormalizedCoordinates(){
    m_info.unnormalizedCoordinates = true;
    return *this;
}
VkSampler SamplerInfo::create(){
    return g_allocator.get().createSampler(m_info);
}

