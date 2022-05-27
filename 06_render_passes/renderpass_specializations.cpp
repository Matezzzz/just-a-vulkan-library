#include "renderpass_specializations.h"


SimpleRenderPassInfo::SimpleRenderPassInfo() : RenderPassInfo(1){} 

SimpleRenderPassInfo& SimpleRenderPassInfo::addColorAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentStoreOp store_op){
    return addColorAttachment(format, final_layout, RenderPassAttType::Color.default_load_op, store_op);
}
SimpleRenderPassInfo& SimpleRenderPassInfo::addColorAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op){
    return addAttachment(format, RenderPassAttType::Color, final_layout, load_op, store_op);
}
SimpleRenderPassInfo& SimpleRenderPassInfo::addDepthAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentStoreOp store_op)
{
    return addAttachment(format, RenderPassAttType::Depth, final_layout, store_op);    
}
SimpleRenderPassInfo& SimpleRenderPassInfo::addDepthStencilAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentStoreOp store_op){
    return addAttachment(format, RenderPassAttType::DepthStencil, final_layout, store_op);
}
SimpleRenderPassInfo& SimpleRenderPassInfo::addAttachment(VkFormat format, const RenderPassAttachmentTypeClass& type, VkImageLayout final_layout, VkAttachmentStoreOp store_op){
    return addAttachment(format, type, final_layout, type.default_load_op, store_op);
}
SimpleRenderPassInfo& SimpleRenderPassInfo::addAttachment(VkFormat format, const RenderPassAttachmentTypeClass& type, VkImageLayout final_layout, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op){
    RenderPassInfo::addAttachment(format, load_op, store_op, type.pass_layout, final_layout);
    if (type.isColor()){
        linkColorAttachment(0, getAttachmentCount()-1, type.pass_layout);
    }else if (type.isDepth()){
        linkDepthAttachment(0, getAttachmentCount()-1, type.pass_layout);
    }else{
        DEBUG_ERROR("Attachment type not recognized")
    }
    return *this;
}
