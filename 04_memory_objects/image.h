#ifndef IMAGE_H
#define IMAGE_H

#include "../00_base/vulkan_base.h"
#include "../09_utilities/vector_ext.h"

/**
 * Represents image format.
 */
class Format{
    VkFormat m_format;
public:
    Format(VkFormat format);
    VkFormat get() const;
    operator VkFormat() const;
    //get size in bytes of one pixel of this format
    uint8_t getSize() const;
};



constexpr VkImageLayout LAYOUT_NOT_USED_YET = VK_IMAGE_LAYOUT_MAX_ENUM;
class ImageState{
public:
    VkImageLayout layout;
    VkAccessFlags access;

    constexpr ImageState() : ImageState(VK_IMAGE_LAYOUT_UNDEFINED, 0)
    {}
    constexpr ImageState(VkImageLayout layout_, VkAccessFlags access_) : layout(layout_), access(access_)
    {}
    bool operator==(const ImageState& s);
    bool operator!=(const ImageState& s);
    string toString() const;
};


//A class with several basic ImageState variables for easy access
class ImgState{
public:
    constexpr static ImageState Invalid         {VK_IMAGE_LAYOUT_MAX_ENUM, 0};
    constexpr static ImageState NewlyCreated    {VK_IMAGE_LAYOUT_UNDEFINED, 0};
    constexpr static ImageState Undefined       {VK_IMAGE_LAYOUT_UNDEFINED, 0};
    constexpr static ImageState Sampled         {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT};
    constexpr static ImageState StorageR        {VK_IMAGE_LAYOUT_GENERAL, VK_ACCESS_SHADER_READ_BIT};
    constexpr static ImageState StorageW        {VK_IMAGE_LAYOUT_GENERAL, VK_ACCESS_SHADER_WRITE_BIT};
    constexpr static ImageState StorageRW       {VK_IMAGE_LAYOUT_GENERAL, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT};
    constexpr static ImageState TransferSrc     {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT};
    constexpr static ImageState TransferDst     {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT};
    constexpr static ImageState ColorAttachment {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT};
    constexpr static ImageState DepthAttachment {VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT};
    constexpr static ImageState DepthStencilAttachment{VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT};
};






class Image;


/**
 * ImageView
 *  - Represents a view into an image
 *  - View can be used to manipulate just a part of an image
 *  - Is used more commonly than raw image in most vulkan functions
 *  - This object becomes invalid after parent image is destroyed
 */
class ImageView{
    VkImageView m_view;
    //pointer to the image, from which this view was created
    const Image* m_parent_image;
public:
    //Create invalid image view
    ImageView();
    //Create image view using given handle and image
    ImageView(VkImageView view, const Image& image);

    operator VkImageView() const;

    //get format of parent image
    const Format& getFormat() const;

    //Get aspect of parent image
    VkImageAspectFlags getAspect() const;

    //get a reference to parent image
    const Image& getImage() const;

    //true if m_view is not VK_NULL_HANDLE
    bool isValid() const;
};




/**
 * Image
 *  - Represents one image
 */
class Image{
    VkImage m_image;
    //whether image is 1D, 2D, ...
    VkImageType m_type;
    VkExtent3D m_size;
    Format m_format;

    //Whether the image is depth, stencil, color, ...
    VkImageAspectFlags m_aspect;
public:
    //creates an invalid image, used by some classes
    Image();
    //create image wrapper for given image with given creation info 
    Image(VkImage image, const VkImageCreateInfo& info);
    
    /**
     * Create memory barrier on the image. This typically signifies that the image has to transition to new access and layout.
     * @param state the layout and access the image is currently in
     * @param new_state  the layout and access to transition to
     * @param current_q_family the index of the current queue family that owns the image. Can be omitted.
     * @param new_q_family the index of the next queue family that should own the image. Can be omitted.
     */
    VkImageMemoryBarrier createMemoryBarrier(ImageState state, ImageState new_state, uint32_t current_q_family=VK_QUEUE_FAMILY_IGNORED, uint32_t new_q_family=VK_QUEUE_FAMILY_IGNORED) const;

    /**
     * Create image view of same format as image.
     * @param view_type what format the view should have. This can be omitted to use the same type the image has. Different settings can be used to create cubemap views and such. Accepted values are VK_IMAGE_VIEW_TYPE_***
     */
    ImageView createView(VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM) const;
    
    /**
     * Create image view of the given format.
     * @param format
     * @param view_type what format the view should have. This can be omitted to use the same type the image has. Different settings can be used to create cubemap views and such. Accepted values are VK_IMAGE_VIEW_TYPE_***
     */
    ImageView createView(VkFormat format, VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM) const; 

    VkImage get() const;
    operator const VkImage&() const;
    const VkExtent3D& getSize() const;
    const Format& getFormat() const;
    VkImageAspectFlags getAspect() const;

    //true if m_image isn't VK_NULL_HANDLE
    bool isValid() const;

    //Get the size the image will occupy in bytes. This is used mainly to determine size for copying operations
    VkDeviceSize getSizeInBytes() const;
    //Get memory requirements. This is used when allocating memory for the image.
    VkMemoryRequirements getMemoryRequirements() const;
};



/**
 * ExtImage
 *  - Holds an image and its' view
 */
class ExtImage : public Image{
    ImageView m_view;
public:
    //create invalid ExtImage
    ExtImage();

    //create ExtImage with given image and given info
    ExtImage(VkImage image, const VkImageCreateInfo& info);

    //return a view if it has been created before, create and return it otherwise
    operator const ImageView&();

    //return a view if it has been created before, create and return it otherwise
    operator VkImageView();

    //return a view if it has been created before, create and return it otherwise
    const ImageView& view();
};




/**
 * ImageMemoryObject
 *  - Is used to allocate memory with given properties for each of the images given
 */
class ImageMemoryObject{
    VkDeviceMemory m_memory;
public:
    //create invalid memory object
    ImageMemoryObject();
    
    /**
     * @brief Assign memory to all images in given vector
     * 
     * @param images the images to assign memory to
     * @param memory_properties properties of the memory, VK_MEMORY_PROPERTY_***
     */
    ImageMemoryObject(const vector_ext<Image>& images, VkMemoryPropertyFlagBits memory_properties);
    
    /**
     * @brief Assign memory to all ExtImage images in given vector
     * 
     * @param images the images to assign memory to
     * @param memory_properties properties of the memory, VK_MEMORY_PROPERTY_***
     */
    ImageMemoryObject(const vector_ext<ExtImage>& images, VkMemoryPropertyFlagBits memory_properties);
    
    /**
     * @brief Assign memory to all images in given vector
     * 
     * @param images the images to assign memory to
     * @param memory_properties properties of the memory, VK_MEMORY_PROPERTY_***
     */
    ImageMemoryObject(const vector<const Image*>& images, VkMemoryPropertyFlagBits memory_properties);
};

#endif