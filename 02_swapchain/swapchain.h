#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H


/**
 * swapchain.h
 *  - SwapchainImage and Swapchain classes
 */


#include "../00_base/vulkan_base.h"
#include "../03_commands/synchronization.h"
#include "../04_memory_objects/image_info.h"



/**
 * SwapchainImage
 *  - One swapchain image and the framebuffer associated with it
 */
const uint32_t IMAGE_INDEX_INVALID = ~0U;
class SwapchainImage : public Image{
    VkFramebuffer m_framebuffer;
    //index of current image in the array of all swapchain images
    uint32_t m_image_index;
public:
    //creates an invalid swapchain image
    SwapchainImage();
    //construct a swapchain image from given image, framebuffer and index
    SwapchainImage(const Image& image, VkFramebuffer framebuffer, uint32_t index);
    operator VkImage() const;
    uint32_t getIndex() const;
    VkFramebuffer getFramebuffer() const;
};


class Queue;
/**
 * Swapchain
 *  - Holds one swapchain
 */
class Swapchain{
    VkSwapchainKHR m_swapchain;
    VkDevice m_device;
    //fence for which to wait when acquiring images
    Fence m_image_acquire_fence;
    //vector of all images and their framebuffers
    vector<VkImage> m_images;
    vector<VkFramebuffer> m_framebuffers;
    //information about one images in the swapchain(format, size, ...)
    ImageInfo m_swapchain_image_info;
public:
    //Set m_swapchain handle and get all swapchain images into 
    Swapchain(VkSwapchainKHR swapchain, const ImageInfo& image_info);

    //Acquire an image to draw into. By default, all images represented by handles in swapchain are under vulkan's control, this makes vulkan turn one of them over to the program.
    SwapchainImage acquireImage();

    //Use given queue to present given image to the screen. Queue must support image presentation.
    void presentImage(const SwapchainImage& img, const Queue& queue);

    //Create framebuffers for all swapchain images, using given render_pass and depth attachment, if needed
    void createFramebuffers(VkRenderPass render_pass, VkImageView depth_attachment = VK_NULL_HANDLE, uint32_t layer_count = 1);

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    VkFormat getFormat() const;
};




#endif