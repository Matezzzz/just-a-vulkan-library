#ifndef IMAGE_INFO_H
#define IMAGE_INFO_H

#include "../00_base/vulkan_base.h"
#include "image.h"
#include <glm/glm.hpp>

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
     * @param data_type pixel format. Accepted values VK_FORMAT_***. If left undefined, must be set before creating.
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***. If left as 0, should be set before creating.
     */
    ImageInfo(uint32_t width, VkFormat data_type = VK_FORMAT_UNDEFINED, VkImageUsageFlags usage = 0);

    /**
     * create 2D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param width size in x direction
     * @param height size in y direction
     * @param data_type pixel format. Accepted values VK_FORMAT_***. If left undefined, must be set before creating.
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***. If left as 0, should be set before creating.
     */
    ImageInfo(uint32_t width, uint32_t height, VkFormat data_type = VK_FORMAT_UNDEFINED, VkImageUsageFlags usage = 0);

    /**
     * create 3D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param width size in x direction
     * @param height size in y direction
     * @param depth size in z direction
     * @param data_type pixel format. Accepted values VK_FORMAT_***. If left undefined, must be set before creating.
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***. If left as 0, should be set before creating.
     */
    ImageInfo(uint32_t width, uint32_t height, uint32_t depth, VkFormat data_type = VK_FORMAT_UNDEFINED, VkImageUsageFlags usage = 0);

    /**
     * create 2D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param size size in pixels
     * @param data_type pixel format. Accepted values VK_FORMAT_***. If left undefined, must be set before creating.
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***. If left as 0, should be set before creating.
     */
    ImageInfo(const glm::uvec2& size, VkFormat data_type = VK_FORMAT_UNDEFINED, VkImageUsageFlags usage = 0);

    /**
     * create 3D image. Default settings:
     *  - no flags
     *  - 1 mipmap level, 1 array level, 1 sample per pixel
     *  - optimal tiling, exclusive sharing mode, undefined default layout
     * @param size size in pixels
     * @param data_type pixel format. Accepted values VK_FORMAT_***. If left undefined, must be set before creating.
     * @param usage how the image will be used. Accepted values VK_IMAGE_USAGE_***. If left as 0, should be set before creating.
     */
    ImageInfo(const glm::uvec3& size, VkFormat data_type = VK_FORMAT_UNDEFINED, VkImageUsageFlags usage = 0);

    //create a copy of this image info and return it
    ImageInfo copy() const;

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
     * Make image info represent a 1D image with given width.
     * @param w new width
     */
    ImageInfo& setSize(uint32_t w);

    /**
     * Make image info represent a 2D image with given width and height.
     * @param w new width
     * @param h new height
     */
    ImageInfo& setSize(uint32_t w, uint32_t h);

    /**
     * Make image info represent a 3D image with given width, height and depth.
     * @param w new width
     * @param h new height
     * @param d new depth
     */
    ImageInfo& setSize(uint32_t w, uint32_t h, uint32_t d);

    /**
     * Change image type to 2D with given (width, height)
     * @param size vec2(width, height) in pixels
     */
    ImageInfo& setSize(const glm::uvec2& size);

    /**
     * Change image type to 3D with given (width, height, depth)
     * @param size vec3(width, height, depth) in pixels
     */
    ImageInfo& setSize(const glm::uvec3& size);

    /**
     * Set image element data type
     * @param format new data type. Accepted values VK_FORMAT_***
     */
    ImageInfo& setFormat(VkFormat format);

    /**
     * Set how the image will be used/
     * @param usage new usage. Accepted values - binary or of VK_IMAGE_USAGE_***
     */
    ImageInfo& setUsage(VkImageUsageFlags usage);

    /**
     * Modify how the image will be used - add given usages to the list
     * @param usage the usages to add. Accepted values - binary or of VK_IMAGE_USAGE_***
     */
    ImageInfo& addUsage(VkImageUsageFlags usage);

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

    //create image without a view, with given settings
    Image createImage() const;

    //create image with view
    ExtImage create() const;
};

#endif