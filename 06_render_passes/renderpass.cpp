#include "renderpass.h"
#include "../01_device/allocator.h"

ClearValue::ClearValue(uint32_t r, uint32_t g, uint32_t b, uint32_t a) : m_value{.color{.uint32{r, g, b, a}}}
{}
ClearValue::ClearValue(int32_t r, int32_t g, int32_t b, int32_t a) : m_value{.color{.int32{r, g, b, a}}}
{}
ClearValue::ClearValue(float r, float g, float b, float a) : m_value{.color{.float32{r, g, b, a}}}
{}
ClearValue::ClearValue(float depth, uint32_t stencil) : m_value{.depthStencil{depth, stencil}}
{}
ClearValue::operator const VkClearValue&() const{
    return m_value;
}



ClearDepthValue::ClearDepthValue(float depth, uint32_t stencil) : ClearValue{depth, stencil}
{}



RenderPassSettings::RenderPassSettings(uint32_t width, uint32_t height, const vector<ClearValue>& clear_values) :
    m_clear_values(clear_values.size()),
    //VK_NULL_HANDLE is given for render pass and framebuffer, these are set when beginning the render pass
    m_begin_info{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO, nullptr, VK_NULL_HANDLE, VK_NULL_HANDLE, VkRect2D{0, 0, width, height}, (uint32_t) m_clear_values.size(), m_clear_values.data()}
{
    //save given clear values (they have to be converted from ClearValue to VkClearValue, cannot be done in constructor)
    for (uint32_t i = 0; i < clear_values.size(); i++){
        m_clear_values[i] = clear_values[i];
    }
}
const VkRenderPassBeginInfo& RenderPassSettings::getBeginInfo(VkRenderPass render_pass, VkFramebuffer framebuffer){
    m_begin_info.renderPass = render_pass;
    m_begin_info.framebuffer = framebuffer;
    return m_begin_info;
}






RenderPassInfo::RenderPassInfo(uint32_t subpass_count) :
    //the 0 + nullptr params describe different type of attachments for each subpass, these are added before creating render pass
    m_subpass_descriptions(subpass_count, VkSubpassDescription{0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr, 0, nullptr, nullptr, nullptr, 0, nullptr}),\
    m_input_attachments(subpass_count), m_color_attachments(subpass_count), m_depth_attachments(subpass_count), m_preserve_attachments(subpass_count)
{}
RenderPassInfo& RenderPassInfo::addAttachment(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkSampleCountFlagBits samples){
    //addAttachmentDepthStencil does the same as addAttachment, but allows more parameters to specify behaviour for depth and stencil images. Set them to DONT_CARE and call the other func.
    return addAttachmentDepthStencil(format, load_op, store_op, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, initial_layout, final_layout, samples);
}
RenderPassInfo& RenderPassInfo::addAttachmentDepthStencil(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op, VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkSampleCountFlagBits samples){
    //add attachment description to the list
    m_attachments.push_back(
        VkAttachmentDescription{
            0, format, samples,
            load_op, store_op, stencil_load_op, stencil_store_op,
            initial_layout, final_layout
        }
    );
    return *this;
}
//RenderPassInfo& RenderPassInfo::addSubpassDependency();

RenderPassInfo& RenderPassInfo::linkInputAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout){
    //add input attachment to given subpass, then update pointer and count of input attachments in subpass structure
    m_input_attachments[subpass_index].push_back(VkAttachmentReference{attachment_index, attachment_layout});
    m_subpass_descriptions[subpass_index].inputAttachmentCount = m_input_attachments[subpass_index].size();
    m_subpass_descriptions[subpass_index].pInputAttachments = m_input_attachments[subpass_index].data();
    return *this;
}
RenderPassInfo& RenderPassInfo::linkColorAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout){
    //add color attachment to given subpass, then update pointer and count of color attachments in subpass structure
    m_color_attachments[subpass_index].push_back(VkAttachmentReference{attachment_index, attachment_layout});
    m_subpass_descriptions[subpass_index].colorAttachmentCount = m_color_attachments[subpass_index].size();
    m_subpass_descriptions[subpass_index].pColorAttachments = m_color_attachments[subpass_index].data();
    return *this;
}
RenderPassInfo& RenderPassInfo::linkDepthAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout){\
    //add depth attachment to the given subpass, then set a pointer to it in subpass structure
    m_depth_attachments[subpass_index] = VkAttachmentReference{attachment_index, attachment_layout};
    m_subpass_descriptions[subpass_index].pDepthStencilAttachment = &m_depth_attachments[subpass_index];
    return *this;
}
RenderPassInfo& RenderPassInfo::linkPreserveAttachment(uint32_t subpass_index, uint32_t attachment_index){
    //add preserve attachment to the given subpass, then update pointer and count of preserve attachments in given subpass
    m_preserve_attachments[subpass_index].push_back(attachment_index);
    m_subpass_descriptions[subpass_index].preserveAttachmentCount = m_preserve_attachments[subpass_index].size();
    m_subpass_descriptions[subpass_index].pPreserveAttachments = m_preserve_attachments[subpass_index].data();
    return *this;
}
VkRenderPass RenderPassInfo::create(){
    //copy all renderpass creation info into VkRenderPassCreateInfo structure, then create render pass
    VkRenderPassCreateInfo info{
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0,
        (uint32_t) m_attachments.size(), m_attachments.data(),
        (uint32_t) m_subpass_descriptions.size(), m_subpass_descriptions.data(),
        (uint32_t) m_subpass_dependencies.size(), m_subpass_dependencies.data()
    };
    return g_allocator.get().createRenderPass(info);
}