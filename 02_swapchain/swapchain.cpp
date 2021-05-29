#include "swapchain.h"
#include "../01_device/device.h"
#include "../06_render_passes/framebuffer.h"

SwapchainImage::SwapchainImage() : Image{}, m_image_index(IMAGE_INDEX_INVALID)
{}
SwapchainImage::SwapchainImage(const Image& image, VkFramebuffer framebuffer, uint32_t index) : Image(image), m_framebuffer(framebuffer), m_image_index(index)
{}
uint32_t SwapchainImage::getIndex() const{
    //if the image wasn't initialized with valid values, print error and return invalid index
    if (m_image_index == IMAGE_INDEX_INVALID){
        PRINT_ERROR("Asking for index of invalid swapchain image")
        return ~0U;
    }
    return m_image_index;
}
VkFramebuffer SwapchainImage::getFramebuffer() const{
    return m_framebuffer;
}


Swapchain::Swapchain(VkSwapchainKHR swapchain, const ImageInfo& image_info) :
    m_swapchain(swapchain), m_device(g_device), m_swapchain_image_info(image_info)
{
    //get actual swapchain image count - this can be larger than count specified by swapchainInfo
    uint32_t swapchain_image_count;
    VkResult result = vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchain_image_count, nullptr);
    DEBUG_CHECK("Get swapchain image count", result)

    //retrieve all swapchain images
    m_images.resize(swapchain_image_count, VK_NULL_HANDLE);
    result = vkGetSwapchainImagesKHR(m_device, m_swapchain, &swapchain_image_count, m_images.data());
    DEBUG_CHECK("Get swapchain images", result);
}

SwapchainImage Swapchain::acquireImage(){
    //call vulkan func to retrieve index of image that can be used
    uint32_t image_index;
    //wait 10us for image to be available, if none is 
    VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, SYNC_10US, VK_NULL_HANDLE, VK_NULL_HANDLE, &image_index);
    //if image was succesfully returned, return it
    if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR){
        return SwapchainImage{Image{m_images[image_index], m_swapchain_image_info}, m_framebuffers[image_index], image_index};
    }
    //sometimes swapchain can break and invalidate all images. I don't know the conditions under which it happens, so there is no protection against it.
    else if (result == VK_ERROR_OUT_OF_DATE_KHR){
        PRINT_ERROR("Have to create new swapchain. This one is broken")
        throw std::runtime_error("Swapchain broken (VK_ERROR_OUT_OF_DATE_KHR)");
    }
    //if no images are ready yet
    PRINT_WARN("No image is ready to be acquired")
    return SwapchainImage{};
}
void Swapchain::presentImage(const SwapchainImage& img, const Queue& queue){
    //fill image present info
    uint32_t index = img.getIndex();
    VkPresentInfoKHR present_info{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR, nullptr, 0, nullptr, 1, &m_swapchain, &index, nullptr};

    //send the given image to be displayed
    VkResult result = vkQueuePresentKHR(queue, &present_info);
    DEBUG_CHECK("Present image", result)
}
void Swapchain::createFramebuffers(VkRenderPass render_pass, VkImageView depth_attachment, uint32_t layer_count){
    //allocate space for the framebuffers
    m_framebuffers.resize(m_images.size(), VK_NULL_HANDLE);
    //go through all images
    for (uint32_t i = 0; i < m_images.size(); i++){
        //create image view for the given image
        VkImageView color_attachment = Image{m_images[i], m_swapchain_image_info}.createView();
        
        //create framebuffer for the given image and save it
        m_framebuffers[i] = FramebufferInfo(getWidth(), getHeight(),
            depth_attachment == VK_NULL_HANDLE ? vector<VkImageView>{color_attachment} : vector<VkImageView>{color_attachment, depth_attachment},
            render_pass).setLayerCount(layer_count).create();
        }
}
uint32_t Swapchain::getWidth() const
{
    return m_swapchain_image_info.get().extent.width;
}
uint32_t Swapchain::getHeight() const
{
    return m_swapchain_image_info.get().extent.height;
}
VkFormat Swapchain::getFormat() const
{
    return m_swapchain_image_info.get().format;
}
