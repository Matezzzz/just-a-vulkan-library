#ifndef RENDERPASS_SPECIALIZATIONS_H
#define RENDERPASS_SPECIALIZATIONS_H

#include "renderpass.h"
#include "../04_memory_objects/image_info.h"


struct RenderPassAttachmentTypeClass{
public:
    enum Type{
        COLOR, DEPTH, DEPTH_STENCIL
    } const m_type;

    const VkImageLayout pass_layout;
    const VkAttachmentLoadOp default_load_op;
    const VkAttachmentStoreOp default_store_op;
    const ImageState pipeline_image_state;
    const VkPipelineStageFlags pipeline_first_use;
    const VkPipelineStageFlags pipeline_last_use;
    const ClearValue default_clear_value;
public:
    constexpr RenderPassAttachmentTypeClass(Type type, VkImageLayout layout, VkAttachmentLoadOp load, VkAttachmentStoreOp store, ImageState pipeline_img_state, VkPipelineStageFlags first_use, VkPipelineStageFlags last_use, const ClearValue& clear_c) :
        m_type(type), pass_layout{layout}, default_load_op{load}, default_store_op{store}, pipeline_image_state{pipeline_img_state}, pipeline_first_use{first_use}, pipeline_last_use{last_use}, default_clear_value{clear_c}
    {}
    constexpr bool isColor() const { return m_type == COLOR;}
    constexpr bool isDepth() const { return m_type == DEPTH;}
};


//Holds basic RenderPassAttachmentType variables
class RenderPassAttType{
public:
    static constexpr RenderPassAttachmentTypeClass Color{
        RenderPassAttachmentTypeClass::Type::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE,
        ImgState::ColorAttachment, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        BaseClearValue::BlackOpaque
    };
    static constexpr RenderPassAttachmentTypeClass Depth{
        RenderPassAttachmentTypeClass::Type::DEPTH, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        ImgState::DepthAttachment, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        BaseClearValue::Depth1f
    };
    static constexpr RenderPassAttachmentTypeClass DepthStencil{
        RenderPassAttachmentTypeClass::Type::DEPTH_STENCIL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE,
        ImgState::DepthStencilAttachment, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        BaseClearValue::DepthStencil1f0
    };
};





/**
 * SimpleRenderPassInfo
 *  - initializes a renderpass with one color attachment and optionally one depth attachment
 *  - color attachment is cleared before using and contents are kept after render pass
 *  - depth attachment is cleared before using and discarded when ending render pass
 */
class SimpleRenderPassInfo : public RenderPassInfo{
public:
    //Create a RenderPassInfo with one subpass
    SimpleRenderPassInfo();

    /**
     * @brief Add a color attachment. Index will be the first one available.
     * 
     * @param format color attachment format
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param store_op with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     */
    SimpleRenderPassInfo& addColorAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentStoreOp store_op = RenderPassAttType::Color.default_store_op);

    /**
     * @brief  Add a color attachment. Index will be the first one available.
     * 
     * @param format color attachment format
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param load_op what operation should be performed on loading. VK_ATTACHMENT_LOAD_OP_***
     * @param store_op with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     */
    SimpleRenderPassInfo& addColorAttachment(VkFormat format, VkImageLayout final_layout, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op = RenderPassAttType::Color.default_store_op);

    /**
     * @brief Add a depth attachment.
     * 
     * @param format depth attachment format
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param store_op with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     */
    SimpleRenderPassInfo& addDepthAttachment(VkFormat format, VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED, VkAttachmentStoreOp store_op = RenderPassAttType::Depth.default_store_op);

    /**
     * @brief Add a depth & stencil attachment
     * 
     * @param format depth & stencil attachment format
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param store_op with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     */
    SimpleRenderPassInfo& addDepthStencilAttachment(VkFormat format, VkImageLayout final_layout = VK_IMAGE_LAYOUT_UNDEFINED, VkAttachmentStoreOp store_op = RenderPassAttType::Depth.default_store_op);

    /**
     * @brief Add an attachment of given type
     * 
     * @param format attachment format
     * @param type most likely RenderPassAttType::Color/Depth/DepthStencil
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param store_op  with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     */
    SimpleRenderPassInfo& addAttachment(VkFormat format, const RenderPassAttachmentTypeClass& type, VkImageLayout final_layout, VkAttachmentStoreOp store_op = RenderPassAttType::Color.default_store_op);

    /**
     * @brief Add an attachment of given type
     * 
     * @param format attachment format
     * @param type most likely RenderPassAttType::Color/Depth/DepthStencil
     * @param final_layout the layout to transition the image into after the render pass ends
     * @param load_op what operation should be performed on loading. VK_ATTACHMENT_LOAD_OP_***
     * @param store_op with what operation should the image be stored. VK_ATTACHMENT_STORE_OP_***
     * @return SimpleRenderPassInfo& 
     */
    SimpleRenderPassInfo& addAttachment(VkFormat format, const RenderPassAttachmentTypeClass& type, VkImageLayout final_layout, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op = RenderPassAttType::Color.default_store_op);
};  

#endif