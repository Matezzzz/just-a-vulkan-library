#ifndef RENDERPASS_SPECIALIZATIONS_H
#define RENDERPASS_SPECIALIZATIONS_H

#include "renderpass.h"


/**
 * SimpleRenderPassInfo
 *  - initializes a renderpass with one color attachment and optionally one depth attachment
 *  - color attachment is cleared before using and contents are kept after render pass
 *  - depth attachment is cleared before using and discarded when ending render pass
 */
class SimpleRenderPassInfo : public RenderPassInfo{
public:
    SimpleRenderPassInfo(VkFormat color_attachment_format, VkImageLayout final_layout);
    SimpleRenderPassInfo(VkFormat color_attachment_format, VkImageLayout final_layout, VkFormat depth_stencil_format, VkImageLayout final_depth_layout = VK_IMAGE_LAYOUT_UNDEFINED);
};

#endif