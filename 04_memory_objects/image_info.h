#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

#include "../00_base/vulkan_base.h"
#include "image.h"

/**
 * ImageInfo
 *  - holds all information for creating an image
 */
class ImageInfo{
protected:
    VkImageCreateInfo m_info;
public:
    /**
     * create 1D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param width size
     * @param data_type pixel format. Accepted values VK_FORMAT_***
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***
     */
    ImageInfo(uint32_t width, VkFormat data_type, VkImageUsageFlags usage);

    /**
     * create 2D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param width size in x direction
     * @param height size in y direction
     * @param data_type pixel format. Accepted values VK_FORMAT_***
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***
     */
    ImageInfo(uint32_t width, uint32_t height, VkFormat data_type, VkImageUsageFlags usage);

    /**
     * create 3D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param width size in x direction
     * @param height size in y direction
     * @param depth size in z direction
     * @param data_type pixel format. Accepted values VK_FORMAT_***
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***
     */
    ImageInfo(uint32_t width, uint32_t height, uint32_t depth, VkFormat data_type, VkImageUsageFlags usage);

    /**
     * Set image creation flags
     * @param flags Accepted values - any bit combination of VK_IMAGE_CREATE_***
     */
    ImageInfo& setFlags(VkImageCreateFlags flags);

    //Set the number of mipmaps the image should have    
    ImageInfo& setMipmapCount(uint32_t mipmap_count);

    //Set the number of array layers the image should have
    ImageInfo& setLayerCount(uint32_t layer_count);

    /**
     * Set the number of samples per pixel.
     * @param samples accepted values VK_SAMPLE_COUNT_***
     */
    ImageInfo& setSamples(VkSampleCountFlagBits samples);

    //Pixels are laid out as decided by implementation, optimized for speed
    ImageInfo& setTilingOptimal();

    //Pixels are laid out in memory linearly, in row-major order, possibly with padding per row
    ImageInfo& setTilingLinear();

    /**
     * Set default image layout
     * WARNING - must be VK_IMAGE_LAYOUT_UNDEFINED or VK_IMAGE_LAYOUT_PREINITIALIZED
     * @param layout
     */
    ImageInfo& setLayout(VkImageLayout layout);

    const VkImageCreateInfo& get() const;
    operator const VkImageCreateInfo&() const;

    Image createImage() const;

    //create image with view
    ExtImage create() const;
};

#endif