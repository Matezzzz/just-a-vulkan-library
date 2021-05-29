#include "image_info.h"

#include "../01_device/allocator.h"



//Image properties are described better in the header, no need to explain them here
ImageInfo::ImageInfo(uint32_t width, VkFormat data_format, VkImageUsageFlags usage) :
    m_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, 0, VK_IMAGE_TYPE_1D, data_format, {width, 1, 1}, 1, 1, 
        VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_UNDEFINED}
{}
ImageInfo::ImageInfo(uint32_t width, uint32_t height, VkFormat data_format, VkImageUsageFlags usage)
    : m_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, 0, VK_IMAGE_TYPE_2D, data_format, {width, height, 1}, 1, 1, 
        VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_UNDEFINED}
{}
ImageInfo::ImageInfo(uint32_t width, uint32_t height, uint32_t depth, VkFormat data_format, VkImageUsageFlags usage)
    : m_info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO, nullptr, 0, VK_IMAGE_TYPE_3D, data_format, {width, height, depth}, 1, 1, 
        VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, usage, VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_IMAGE_LAYOUT_UNDEFINED}
{}

ImageInfo& ImageInfo::setFlags(VkImageCreateFlags flags){
    m_info.flags = flags;
    return *this;
}
ImageInfo& ImageInfo::setMipmapCount(uint32_t mipmap_count){
    m_info.mipLevels = mipmap_count;
    return *this;
}
ImageInfo& ImageInfo::setLayerCount(uint32_t layer_count){
    m_info.arrayLayers = layer_count;
    return *this;
}
ImageInfo& ImageInfo::setSamples(VkSampleCountFlagBits samples){
    m_info.samples = samples;
    return *this;
}
ImageInfo& ImageInfo::setTilingOptimal(){
    m_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    return *this;
}
ImageInfo& ImageInfo::setTilingLinear(){
    m_info.tiling = VK_IMAGE_TILING_LINEAR;
    return *this;
}
ImageInfo& ImageInfo::setLayout(VkImageLayout layout){
    m_info.initialLayout = layout;
    return *this;
}
const VkImageCreateInfo& ImageInfo::get() const{
    return m_info;
}
ImageInfo::operator const VkImageCreateInfo&() const{
    return m_info;
}
Image ImageInfo::create() const{
    //create image with given info
    return Image{g_allocator.get().createImage(m_info), m_info};
}
