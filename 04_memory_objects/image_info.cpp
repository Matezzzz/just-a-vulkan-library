#include "image_info.h"

#include "../01_device/allocator.h"

#define RTRN return *this;

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
ImageInfo::ImageInfo(const glm::uvec2& size, VkFormat data_type, VkImageUsageFlags usage) : ImageInfo(size.x, size.y, data_type, usage)
{}
ImageInfo::ImageInfo(const glm::uvec3& size, VkFormat data_type, VkImageUsageFlags usage) : ImageInfo(size.x, size.y, size.z, data_type, usage)
{}
ImageInfo ImageInfo::copy() const{
    return *this;
}
ImageInfo& ImageInfo::setFlags(VkImageCreateFlags flags){
    m_info.flags = flags; RTRN
}
ImageInfo& ImageInfo::setMipmapCount(uint32_t mipmap_count){
    m_info.mipLevels = mipmap_count; RTRN
}
ImageInfo& ImageInfo::setLayerCount(uint32_t layer_count){
    m_info.arrayLayers = layer_count; RTRN
}
ImageInfo& ImageInfo::setSize(uint32_t w){
    m_info.extent = VkExtent3D{w, 1, 1};
    m_info.imageType = VK_IMAGE_TYPE_1D;
    RTRN
}
ImageInfo& ImageInfo::setSize(uint32_t w, uint32_t h){
    m_info.extent = VkExtent3D{w, h, 1};
    m_info.imageType = VK_IMAGE_TYPE_2D;
    RTRN
}
ImageInfo& ImageInfo::setSize(uint32_t w, uint32_t h, uint32_t d){
    m_info.extent = VkExtent3D{w, h, d};
    m_info.imageType = VK_IMAGE_TYPE_3D;
    RTRN
}
ImageInfo& ImageInfo::setSize(const glm::uvec2& size){
    return setSize(size.x, size.y);
}

ImageInfo& ImageInfo::setSize(const glm::uvec3& size){
    return setSize(size.x, size.y, size.z);
}
ImageInfo& ImageInfo::setFormat(VkFormat format){
    m_info.format = format; RTRN
}
ImageInfo& ImageInfo::setUsage(VkImageUsageFlags usage){
    m_info.usage = usage; RTRN
}
ImageInfo& ImageInfo::addUsage(VkImageUsageFlags usage){
    m_info.usage |= usage; RTRN
}

ImageInfo& ImageInfo::setSamples(VkSampleCountFlagBits samples){
    m_info.samples = samples; RTRN
}
ImageInfo& ImageInfo::setTilingOptimal(){
    m_info.tiling = VK_IMAGE_TILING_OPTIMAL; RTRN
}
ImageInfo& ImageInfo::setTilingLinear(){
    m_info.tiling = VK_IMAGE_TILING_LINEAR; RTRN
}
ImageInfo& ImageInfo::setLayout(VkImageLayout layout){
    m_info.initialLayout = layout; RTRN
}
const VkImageCreateInfo& ImageInfo::get() const{
    return m_info;
}
ImageInfo::operator const VkImageCreateInfo&() const{
    return m_info;
}
Image ImageInfo::createImage() const{
    //create image with given info
    return Image{g_allocator.get().createImage(m_info), m_info};
}
ExtImage ImageInfo::create() const{
    //create image and view with given info
    return ExtImage{g_allocator.get().createImage(m_info), m_info};
}

#undef RTRN