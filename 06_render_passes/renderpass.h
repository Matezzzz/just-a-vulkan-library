#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "../00_base/vulkan_base.h"


/**
 * ClearValue
 *  - Manages clear value for one layer of framebuffer
 *  - Value can be of type:
 *    - uint, int, float for color layer
 *    - float for depth
 *    - uint32_t for stencil
 */
class ClearValue {
    VkClearValue m_value;
public:
    ClearValue(uint32_t r, uint32_t g = 0, uint32_t b = 0, uint32_t a = 0);
    ClearValue(int32_t r, int32_t g = 0, int32_t b = 0, int32_t a = 0);
    ClearValue(float r, float g = 0, float b = 0, float a = 0);
    ClearValue(float depth, uint32_t stencil);
    operator const VkClearValue&() const;
};



/**
 * ClearDepthValue
 *  - Specialized class derived from ClearValue to initialize depth clear values
 */
class ClearDepthValue : public ClearValue{
public:
    ClearDepthValue(float depth, uint32_t stencil = 0);
};


/**
 * RenderPassSettings
 *  - Contains information to be used while starting renderpass
 */
class RenderPassSettings{
    //one clear value for each image in renderpass
    vector<VkClearValue> m_clear_values;
    VkRenderPassBeginInfo m_begin_info;
public:
    /**
     * Create new renderpass settings.
     * @param width the width of images to render into
     * @param height the height of images to render into
     * @param clear_values values, with which to clear all 
     */
    RenderPassSettings(uint32_t width, uint32_t height, const vector<ClearValue>& clear_values);
    //Fill begin info with given render pass and framebuffer, then return it
    const VkRenderPassBeginInfo& getBeginInfo(VkRenderPass render_pass, VkFramebuffer framebuffer);
};




/**
 * RenderPassInfo
 *  - holds information needed to create renderpass
 *  - renderpass can have multiple subpasses, each one can use attachments for different purposes
 */
class RenderPassInfo{
    //holds description of one image to be used in any of the subpasses
    vector<VkAttachmentDescription> m_attachments;

    vector<VkSubpassDescription> m_subpass_descriptions;

    //what part of previous subpass has to be finished for a part of the next one to start
    vector<VkSubpassDependency> m_subpass_dependencies;
    
    //input attachments can be specified as a descriptor in shaders, they allow texture access, but only to pixel in the same location as the one being rendered
    vector<vector<VkAttachmentReference>> m_input_attachments;

    //attachments to save fragment shader output into
    vector<vector<VkAttachmentReference>> m_color_attachments;

    //one attachment per subpass to save depth information into
    vector<VkAttachmentReference> m_depth_attachments;

    //attachments whose contents to preserve during this subpass
    vector<vector<uint32_t>> m_preserve_attachments;
public:
    //Create without color attachments
    RenderPassInfo(uint32_t subpass_count = 1);

    /**
     * Add given attachment to the renderpass. After being added, it isn't used in any of the subpasses yet. It has to be linked before that.
     * @param format attachment format
     * @param load_op what to do with image before renderpass starts. Possible values VK_ATTACHMENT_LOAD_OP_*** - LOAD, CLEAR, DONT_CARE
     * @param store_op what to do with image after renderpass ends. Possible values VK_ATTACHMENT_STORE_OP_*** - STORE, DONT_CARE
     * @param initial_layout what layout was the image in before render pass
     * @param final_layout what layout the image should be in after ending render pass
     * @param samples the amount of samples per pixel in the attachment, can be omitted
     */
    RenderPassInfo& addAttachment(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    
    /**
     * Add given depth-stencil attachment to the renderpass. After being added, it isn't used in any of the subpasses yet. It has to be linked before that.
     * @param format attachment format
     * @param load_op what to do with depth image before renderpass starts. Possible values VK_ATTACHMENT_LOAD_OP_*** - LOAD, CLEAR, DONT_CARE
     * @param store_op what to do with depth image after renderpass ends. Possible values VK_ATTACHMENT_STORE_OP_*** - STORE, DONT_CARE
     * @param stencil_load_op what to do with stencil image part before renderpass starts. Possible values VK_ATTACHMENT_LOAD_OP_*** - LOAD, CLEAR, DONT_CARE
     * @param stencil_store_op what to do with stencil image part after renderpass ends. Possible values VK_ATTACHMENT_STORE_OP_*** - STORE, DONT_CARE
     * @param initial_layout what layout was the image in before render pass
     * @param final_layout what layout the image should be in after ending render pass
     * @param samples the amount of samples per pixel in the attachment, can be omitted
     */
    RenderPassInfo& addAttachmentDepthStencil(VkFormat format, VkAttachmentLoadOp load_op, VkAttachmentStoreOp store_op, VkAttachmentLoadOp stencil_load_op, VkAttachmentStoreOp stencil_store_op, VkImageLayout initial_layout, VkImageLayout final_layout, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    
    //Subpass dependencies aren't implemented yet
    //RenderPassInfo& addSubpassDependency();

    /**
     * Link previously added attachment to function as an input attachment in the given subpass.
     * @param subpass_index the subpass to add input attachment to
     * @param attachment_index  the index of the attachment that should be linked. Attachments are ordered as they were added
     * @param attachment_layout the layout in which the image should be for this subpass
     */
    RenderPassInfo&   linkInputAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout);

    /**
     * Link previously added attachment to function as an color attachment in the given subpass.
     * @param subpass_index the subpass to add color attachment to
     * @param attachment_index  the index of the attachment that should be linked. Attachments are ordered as they were added.
     * @param attachment_layout the layout in which the image should be for this subpass
     */
    RenderPassInfo&   linkColorAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout);

    /**
     * Link previously added attachment to function as a depth attachment in the given subpass. Only one depth attachment per subpass is possible.
     * @param subpass_index the subpass to set depth attachment for.
     * @param attachment_index  the index of the attachment that should be linked. Attachments are ordered as they were added.
     * @param attachment_layout the layout in which the image should be for this subpass
     */
    RenderPassInfo&   linkDepthAttachment(uint32_t subpass_index, uint32_t attachment_index, VkImageLayout attachment_layout);

    /**
     * Link previously added attachment to be preserved in the given subpass.
     * @param subpass_index the subpass in which to preserve attachment
     * @param attachment_index  the index of the attachment that should be linked. Attachments are ordered as they were added.
     * @param attachment_layout the layout in which the image should be for this subpass
     */
    RenderPassInfo&   linkPreserveAttachment(uint32_t subpass_index, uint32_t attachment_index);

    VkRenderPass create();
};





#endif