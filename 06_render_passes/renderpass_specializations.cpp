#include "renderpass_specializations.h"


SimpleRenderPassInfo::SimpleRenderPassInfo(VkFormat format, VkImageLayout final_layout) : RenderPassInfo(1){
    //add one color attachment to the render pass
    addAttachment(format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, final_layout);
    linkColorAttachment(0, 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}
SimpleRenderPassInfo::SimpleRenderPassInfo(VkFormat color_attachment_format, VkImageLayout final_layout, VkFormat depth_stencil_format, VkImageLayout final_depth_layout) :
    //call other constructor to add color attachment
    SimpleRenderPassInfo(color_attachment_format, final_layout)
{
    //add one depth attachment
    addAttachment(depth_stencil_format, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, final_depth_layout);
    linkDepthAttachment(0, 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}