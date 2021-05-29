#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../00_base/vulkan_base.h"


/**
 * FramebufferInfo
 *  - holds all info required to create the framebuffer
 */
class FramebufferInfo{
    vector<VkImageView> m_image_views;
    VkFramebufferCreateInfo m_info;
public:
    /**
     * Initialize framebuffer info with no flags and 1 layer
     * @param width framebuffer width
     * @param height framebuffer height
     * @param images image views to form framebuffer 
     * @param render_pass the render pass to use framebuffer with. Note that compatible render_passes can be used as well.
     */
    FramebufferInfo(uint32_t width, uint32_t height, const vector<VkImageView>& images, VkRenderPass render_pass, VkFramebufferCreateFlags create_flags = 0);

    /**
     * Set framebuffer create flags
     * @param flags only 0 or VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT can be provided
     */
    FramebufferInfo& setFlags(VkFramebufferCreateFlags flags);
    
    //Set amount of framebuffer layers
    FramebufferInfo& setLayerCount(uint32_t layer_count);
    VkFramebuffer create();
};

#endif