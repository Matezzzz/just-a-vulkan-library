#ifndef SWAPCHAIN_INFO_H
#define SWAPCHAIN_INFO_H

/**
 * swapchain_info.h
 *  - SwapchainInfo class used for swapchain creation
 */

#include "../00_base/vulkan_base.h"
#include "swapchain.h"

class Window;

/**
 * SwapchainInfo
 *  - holds all information required for creating a swapchain and enables user to set swapchain properties
 */
class SwapchainInfo{
    VkPhysicalDevice m_device;
    VkSurfaceKHR m_surface;
    VkSwapchainCreateInfoKHR m_info;
    VkSurfaceCapabilitiesKHR m_capabilities;
public:
    /**
     * Create new swapchainInfo with the following properties
     * - min. 3 images of window size, format R8G8B8A8_SRGB and color space VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
     * - image usage VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
     * - image sharing mode VK_SHARING_MODE_EXCLUSIVE
     * - identity transform VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
     * - VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR
     * - present mode VK_PRESENT_MODE_FIFO_KHR
     * - is clipped
     */
    SwapchainInfo(VkPhysicalDevice device, Window& window);

    /**
     * Check whether passed mode is supported, and set swapchain present mode.
     * @param mode accepted values VK_PRESENT_MODE_***
     */
    SwapchainInfo& setPresentMode(VkPresentModeKHR mode);

    
    /**
     * Check whether given combination of format and color_space is supported, set it if it is
     * @param format accepted values VK_FORMAT_***
     * @param color_space accepted values VK_COLOR_SPACE_***
     */
    SwapchainInfo& setSurfaceFormat(VkFormat format, VkColorSpaceKHR color_space);

    /**
     * Set behavior of alpha values in result texture.
     * @param alpha_mode accepted values VK_COMPOSITE_ALPHA_***
     */
    SwapchainInfo& setCompositeAlphaMode(VkCompositeAlphaFlagBitsKHR alpha_mode);
    
    /**
     * Set min. image count to be allocated alongside swapchain
     * @param image_count
     */
    SwapchainInfo& setImageCount(uint32_t image_count);
    
    /**
     * Set all ways in which the swapchain images will be used.
     * @param usages accepted values VK_IMAGE_USAGE_***
     */
    SwapchainInfo& setUsages(VkImageUsageFlags usages);
    
    /**
     * Set transformation to be applied to images before going on screen.
     * @param transformation accepted values VK_SURFACE_TRANSFORM_***
     */
    SwapchainInfo& setTransformation(VkSurfaceTransformFlagBitsKHR transformation);

    //create swapchain with given info
    Swapchain create();
};


#endif