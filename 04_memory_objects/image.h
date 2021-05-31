#ifndef IMAGE_H
#define IMAGE_H

#include "../00_base/vulkan_base.h"


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
    
    //get current layout of parent image
    //! This is just an attempt to make it possible to omit specifying image layout when calling vulkan functions on an image.
    //! There are cases when this won't represent the actual layout the image is in.
    //! This part of the library should be reworked in the future
    VkImageLayout getLayout() const;

    //Get aspect of parent image
    VkImageAspectFlags getAspect() const;

    //get a reference to parent image
    const Image& getImage() const;
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

    //! This is just an attempt to make it possible to omit specifying image layout when calling vulkan functions on an image.
    //! There are cases when this won't represent the actual layout the image is in.
    //! This part of the library should be reworked in the future
    VkImageLayout m_current_layout;
    VkAccessFlags m_current_access;
    //Whether the image is depth, stencil, color, ...
    VkImageAspectFlags m_aspect;
public:
    //creates an invalid image, used by some classes
    Image();
    //create image wrapper for given image with given creation info 
    Image(VkImage image, const VkImageCreateInfo& info);
    /**
     * Create memory barrier on the image. This typically signifies that the image has to transition to new access and layout. 
     * @param new_layout the layout to transition to
     * @param new_access the types of access after the transition
     * @param current_q_family the index of the current queue family that owns the image. Can be omitted.
     * @param new_q_family the index of the next queue family that should own the image. Can be omitted.
     */
    VkImageMemoryBarrier createMemoryBarrier(VkImageLayout new_layout, VkAccessFlags new_access,
        uint32_t current_q_family=VK_QUEUE_FAMILY_IGNORED, uint32_t new_q_family=VK_QUEUE_FAMILY_IGNORED);
    
    /**
     * Create memory barrier on the image. This typically signifies that the image has to transition to new access and layout.
     * @param layout the layout the image is currently in - m_current_layout is not used as it can be wrong sometimes
     * @param access the access the image is currently using - m_current_access is not used as it can be wrong sometimes
     * @param new_layout the layout to transition to
     * @param new_access the types of access after the transition
     * @param current_q_family the index of the current queue family that owns the image. Can be omitted.
     * @param new_q_family the index of the next queue family that should own the image. Can be omitted.
     */
    VkImageMemoryBarrier createMemoryBarrier(VkImageLayout layout, VkAccessFlags access, VkImageLayout new_layout, VkAccessFlags new_access,
        uint32_t current_q_family=VK_QUEUE_FAMILY_IGNORED, uint32_t new_q_family=VK_QUEUE_FAMILY_IGNORED);


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
    VkImageLayout getLayout() const;
    void setLayout(VkImageLayout layout);
    VkImageAspectFlags getAspect() const;

    //Get the size the image will occupy in bytes. This is used mainly to determine size for copying operations
    VkDeviceSize getSizeInBytes() const;
    //Get memory requirements. This is used when allocating memory for the image.
    VkMemoryRequirements getMemoryRequirements() const;
};



/**
 * ExtImage
 *  - Holds an image and it's view
 */
class ExtImage : public Image{
    ImageView m_view;
public:
    ExtImage(VkImage image, const VkImageCreateInfo& info) : Image(image, info), m_view()
    {}
    operator VkImageView(){
        return view();
    }
    VkImageView view(){
        if (m_view == VK_NULL_HANDLE) m_view = createView();
        return m_view;
    }
};


//return a vector of pointers to each element of vector given
template<typename T>
vector<T*> vectorOfPointers(vector<T>& vec);


/**
 * ImageMemoryObject
 *  - Is used to allocate memory with given properties for each of the images given
 */
class ImageMemoryObject{
    VkDeviceMemory m_memory;
public:
    //create invalid memory object
    ImageMemoryObject();
    ImageMemoryObject(const vector<Image>& images, VkMemoryPropertyFlagBits memory_properties);
    ImageMemoryObject(const vector<const Image*>& images, VkMemoryPropertyFlagBits memory_properties);
};

#endif