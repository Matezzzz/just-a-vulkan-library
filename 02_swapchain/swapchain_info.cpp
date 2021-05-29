#include "swapchain_info.h"

#include "../01_device/allocator.h"
#include "window.h"

SwapchainInfo::SwapchainInfo(VkPhysicalDevice device, Window& window) : 
    m_device(device), m_surface(window.getSurface()),
    m_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR, nullptr, 0,
        window.getSurface(), 3, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
        VkExtent2D{window.getWidth(), window.getHeight()}, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_SHARING_MODE_EXCLUSIVE, 0, nullptr, VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_PRESENT_MODE_FIFO_KHR, true, VK_NULL_HANDLE}
{
    //get device swapchain capabilities and save them
    VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &m_capabilities);
    DEBUG_CHECK("Get surface capabilities", result)
}
SwapchainInfo& SwapchainInfo::setPresentMode(VkPresentModeKHR desired_present_mode){
    //get swapchain present mode count
    uint32_t present_modes_count;
    VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &present_modes_count, nullptr);
    DEBUG_CHECK("Could not get device present modes count", result)

    //get swapchain present modes
    vector<VkPresentModeKHR> present_modes(present_modes_count);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_device, m_surface, &present_modes_count, &present_modes[0]);
    DEBUG_CHECK("Could not retrieve device present modes", result)

    //check if the requested present mode is available, print error if it isn't
    for (VkPresentModeKHR v : present_modes){
        if (v == desired_present_mode)
        {
            m_info.presentMode = desired_present_mode;
            return *this;
        }
    }
    PRINT_ERROR("The requested present mode was not found")
    return *this;
}
SwapchainInfo& SwapchainInfo::setSurfaceFormat(VkFormat desired_format, VkColorSpaceKHR color_space)
{
    //get supported swapchain format count
    uint32_t format_count;
    VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR( m_device, m_surface, &format_count, nullptr );
    DEBUG_CHECK("Could not get format count", result)

    //get supported swapchain formats
    vector<VkSurfaceFormatKHR> available_formats(format_count);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR( m_device, m_surface, &format_count, &available_formats[0] );
    DEBUG_CHECK("Could not retrieve formats", result)

    //index of matching format
    int format_index = -1;
    //go through all available formats
    for (uint32_t i = 0; i < available_formats.size(); i++){
        //if desired format was found
        if (desired_format == available_formats[i].format){
            //if desired format supports desired color space, save it and return, otherwise, save it's index and continue searching 
            if (color_space == available_formats[i].colorSpace){
                m_info.imageFormat = desired_format;
                m_info.imageColorSpace = color_space;
                return *this;
            }else{
                format_index = i;
            }
        }
    }
    //if desired format was not found, print error and return
    if (format_index == -1){
        PRINT_ERROR("Swapchain image format not found");
        throw std::runtime_error("Swapchain image format not found.");
    }
    //If given format was found, but it didn't have the desired color space, print warning and select it
    PRINT_WARN("Swapchain format found, only with different color space")
    m_info.imageFormat = available_formats[format_index].format;
    m_info.imageColorSpace = available_formats[format_index].colorSpace;
    
    return *this;
}
SwapchainInfo& SwapchainInfo::setCompositeAlphaMode(VkCompositeAlphaFlagBitsKHR alpha_mode){
    m_info.compositeAlpha = alpha_mode;
    return *this;
}
SwapchainInfo& SwapchainInfo::setImageCount(uint32_t image_count){
    //if the required image count is within supported bounds, set it, otherwise print error
    if (image_count >= m_capabilities.minImageCount && image_count <= m_capabilities.maxImageCount) m_info.minImageCount = image_count;
    else PRINT_ERROR("Image count out of bounds (" << m_capabilities.minImageCount << ", " << m_capabilities.maxImageCount << ")")
    return *this;
}
SwapchainInfo& SwapchainInfo::setUsages(VkImageUsageFlags desired_usages){
    //if the swapchain supports desired usage, set it, otherwise print error
    if ((m_capabilities.supportedUsageFlags & desired_usages) == desired_usages){
        m_info.imageUsage = desired_usages;
    }else{
        PRINT_ERROR("Requested swapchain usages not available")
    }
    return *this;
}
SwapchainInfo& SwapchainInfo::setTransformation(VkSurfaceTransformFlagBitsKHR desired_transformation){
    //if desired transformation is supported, set it, otherwise print error
    if ((m_capabilities.supportedTransforms & desired_transformation) == desired_transformation) m_info.preTransform = desired_transformation;
    else PRINT_ERROR("Requested swapchain transformation not available")
    return *this;
}
Swapchain SwapchainInfo::create(){
    //create swapchain with info set before, then return it
    return Swapchain(g_allocator.get().createSwapchain(m_info), ImageInfo{m_info.imageExtent.width, m_info.imageExtent.height, m_info.imageFormat, m_info.imageUsage});
}