#include "image.h"

#include "../00_base/vulkan_enum_strings.h"
#include "../01_device/allocator.h"
#include "mixed_buffer.h"


//sizes of each format in bytes
constexpr uint8_t format_sizes[]{0, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6, 6, 6, 8, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 8, 12, 12, 12, 16, 16, 16, 8, 8, 8, 16, 16, 16, 24, 24, 24, 32, 32, 32, 4, 4, 2, 4, 4, 1, 3, 4, 5, 8, 8, 8, 8, 16, 16, 16, 16, 8, 8, 16, 16, 16, 16, 16, 16, 8, 8, 8, 8, 16, 16, 8, 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16};
Format::Format(VkFormat format) : m_format(format)
{}
VkFormat Format::get() const{
    return m_format;
}
Format::operator VkFormat() const{
    return m_format;
}
uint8_t Format::getSize() const{
    //if format is one of the default ones, return one of sizes written above
    if (m_format <= 184){
        return format_sizes[static_cast<uint32_t>(m_format)];
    }
    //for other formats, print error, there was no need to implement this yet
    PRINT_ERROR("Getting size of format for which the function is not implemented yet")
    return 4;   //just guess the size :0
}




ImageState::ImageState(VkImageLayout layout_, VkAccessFlags access_) :
    layout(layout_), access(access_)
{}
ImageState::ImageState(ImageStatesEnum s) : ImageState(image_states_layouts[s], image_states_accesses[s])
{}
bool ImageState::operator==(const ImageState& s){
    return layout == s.layout && access == s.access;
}
bool ImageState::operator!=(const ImageState& s){
    return !(*this == s);
}
string ImageState::toString() const{
    return "(" + VkImageLayoutToString(layout) + ", " + VkAccessFlagsToString(access) + ")";
}






ImageView::ImageView() : m_view(VK_NULL_HANDLE), m_parent_image(nullptr)
{}
ImageView::ImageView(VkImageView view, const Image& image) : m_view(view), m_parent_image(&image)
{}
ImageView::operator VkImageView() const{
    return m_view;
}
const Format& ImageView::getFormat() const{
    return m_parent_image->getFormat();
}
VkImageAspectFlags ImageView::getAspect() const{
    return m_parent_image->getAspect();
}
const Image& ImageView::getImage() const{
    return *m_parent_image;
}



//create invalid image
Image::Image() : m_image(VK_NULL_HANDLE), m_type(VK_IMAGE_TYPE_MAX_ENUM), m_size{0, 0, 0}, m_format(VK_FORMAT_UNDEFINED), m_aspect(0)
{}
Image::Image(VkImage image, const VkImageCreateInfo& info) :
    m_image(image), m_type(info.imageType), m_size(info.extent), m_format(info.format)
{
    //derive image aspect from its' format
    if (m_format == VK_FORMAT_S8_UINT) m_aspect = VK_IMAGE_ASPECT_STENCIL_BIT;
    else if (m_format == VK_FORMAT_D16_UNORM         || m_format == VK_FORMAT_X8_D24_UNORM_PACK32 || m_format == VK_FORMAT_D32_SFLOAT)           m_aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
    else if (m_format == VK_FORMAT_D16_UNORM_S8_UINT || m_format == VK_FORMAT_D24_UNORM_S8_UINT   || m_format == VK_FORMAT_D32_SFLOAT_S8_UINT)   m_aspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    else m_aspect = VK_IMAGE_ASPECT_COLOR_BIT;
}
VkImageMemoryBarrier Image::createMemoryBarrier(ImageState state, ImageState new_state, uint32_t current_q_family, uint32_t new_q_family) const
{
    //create image memory barrier
    VkImageMemoryBarrier barrier
    {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, nullptr, 
        state.access, new_state.access,
        state.layout, new_state.layout,
        current_q_family, new_q_family,
        m_image,
        //         mip level to start from,    array layer to start from
        {m_aspect, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS}
    };
    //return created memory barrier
    return barrier;
}
ImageView Image::createView(VkImageViewType view_type) const{
    return createView(m_format, view_type);
}
ImageView Image::createView(VkFormat format, VkImageViewType view_type) const{
    //convert image type to view type - works for 1D, 2D, 3D images, anything else must pass view type as a parameter
    if (view_type == VK_IMAGE_VIEW_TYPE_MAX_ENUM) view_type = static_cast<VkImageViewType>(m_type);
    //fill view create info
    VkImageViewCreateInfo create_info{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, nullptr, 0,
        m_image, view_type, format,
        //swizzling means swapping components before accessing them, this feature isn't used
        {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        //         no mipmap offset            no layer offset
        {m_aspect, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS}}; 
    return ImageView(g_allocator.get().createImageView(create_info), *this);
}
VkImage Image::get() const{
    return m_image;
}
Image::operator const VkImage& () const{
    return m_image;
} 
const VkExtent3D& Image::getSize() const{
    return m_size;
}
VkDeviceSize Image::getSizeInBytes() const{
    //size_x*size_y*size_z*format_bytes_per_pixel
    return 1ULL*m_size.width * m_size.height * m_size.depth * m_format.getSize();
}
const Format& Image::getFormat() const{
    return m_format;
}
VkImageAspectFlags Image::getAspect() const{
    return m_aspect;
}
VkMemoryRequirements Image::getMemoryRequirements() const{
    //get memory requirements and return them
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(g_device, m_image, &requirements);
    return requirements;
}



template<typename T>
vector<const T*> vectorOfPointers(const vector<T>& vec){
    //create vector of pointers
    vector<const T*> v(vec.size());
    //go through all elements in vec and set each pointer to point to it
    for (uint32_t i = 0; i < vec.size(); i++){
        v[i] = &vec[i];
    }
    //return vector of pointers
    return v;
}



ImageMemoryObject::ImageMemoryObject() : m_memory{VK_NULL_HANDLE}
{}
ImageMemoryObject::ImageMemoryObject(const vector<Image>& images, VkMemoryPropertyFlagBits memory_properties) : ImageMemoryObject(vectorOfPointers(images), memory_properties)
{}
ImageMemoryObject::ImageMemoryObject(const vector<const Image*>& images, VkMemoryPropertyFlagBits memory_properties){
    //total size in bytes
    //VkDeviceSize total_size = 0;
    //all usable memory types
    uint32_t memory_type_bits = 0xFFFFFFFF;
    //one offset per image, last one represents size of all in bytes
    vector<VkDeviceSize> offsets(images.size() + 1, 0);
    offsets[0] = 0;
    VkMemoryRequirements memory_requirements;
    for (uint32_t i = 0; i < images.size(); i++)
    {
        //get image memory requirements - size, required alignment, memory type
        memory_requirements = images[i]->getMemoryRequirements();
        offsets[i] = roundUpToMemoryBlock(offsets[i], memory_requirements.alignment);
        //mark all unusable memory types
        memory_type_bits &= memory_requirements.memoryTypeBits;
        offsets[i+1] = offsets[i] + memory_requirements.size;
    }
    offsets.back() = roundUpToMemoryBlock(offsets.back(), memory_requirements.alignment);
    //allocate memory for all images
    m_memory = g_allocator.get().allocateMemory(offsets.back(), memory_type_bits, memory_properties);
    //bind allocated memory to all images
    for (uint32_t i = 0; i < images.size(); i++){
        VkResult result = vkBindImageMemory(g_device, *images[i], m_memory, offsets[i]);
        DEBUG_CHECK("Image memory binding", result)
    }
}

