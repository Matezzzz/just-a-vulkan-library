#include "framebuffer.h"
#include "../01_device/allocator.h"


FramebufferInfo::FramebufferInfo(uint32_t width, uint32_t height, const vector<VkImageView>& images, VkRenderPass render_pass, VkFramebufferCreateFlags create_flags) :
    m_image_views(images), m_info{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, create_flags, render_pass, (uint32_t) m_image_views.size(), m_image_views.data(), width, height, 1}
{}
FramebufferInfo& FramebufferInfo::setFlags(VkFramebufferCreateFlags flags){
    m_info.flags = flags;
    return *this;
}
FramebufferInfo& FramebufferInfo::setLayerCount(uint32_t layer_count){
    m_info.layers = layer_count;
    return *this;
}
VkFramebuffer FramebufferInfo::create(){
    return g_allocator.get().createFramebuffer(m_info);
}