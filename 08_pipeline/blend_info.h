#ifndef BLEND_INFO_H
#define BLEND_INFO_H

#include "../00_base/vulkan_base.h"

/**
 * BlendState
 *  - This class holds a reference to modify attachment blend options.
 */
class BlendState{
    VkPipelineColorBlendAttachmentState& m_blend_options;
public:
    /**
     * Initalize blend state with given options to modify
     * @param blend_options reference to the blend options
     */
    BlendState(VkPipelineColorBlendAttachmentState& blend_options);

    /**
     * Enable blending in referenced blend options
     */
    BlendState& enableBlending();

    /**
     * Disable blending in referenced blend options
     */
    BlendState& disableBlending();

    /**
     * Set color blend factors and operation.
     * @param source_factor factor with which the source color is modified. Possible values - VK_BLEND_FACTOR_***
     * @param dist_factor factor with which the dist color is modified. Possible values - VK_BLEND_FACTOR_***
     * @param blend_op the operation performed on colors multiplied by factors. Possible values - VK_BLEND_OP_***
     */
    BlendState& setColorBlend(VkBlendFactor source_factor, VkBlendFactor dist_factor, VkBlendOp blend_op);
    
    /**
     * Set alpha blend factors and operation.
     * @param source_factor factor with which the source alpha is modified. Possible values - VK_BLEND_FACTOR_***
     * @param dist_factor factor with which the dist alpha is modified. Possible values - VK_BLEND_FACTOR_***
     * @param blend_op the operation performed on alphas multiplied by factors. Possible values - VK_BLEND_OP_***
     */
    BlendState& setAlphaBlend(VkBlendFactor source_factor, VkBlendFactor dist_factor, VkBlendOp blend_op);
    
    /**
     * Set a write mask to blend only some color channels.
     * @param mask a logical || of VK_COLOR_COMPONENT_*_BIT - R, G, B, A
     */
    BlendState& setWriteMask(VkColorComponentFlags mask);
};


/**
 * BlendInfo
 *  - Controls pipeline blend info
 */
class BlendInfo{
    vector<VkPipelineColorBlendAttachmentState> m_blend_settings;
    VkPipelineColorBlendStateCreateInfo m_info;
public:
    /**
     * Create blend info for given count of attachments.
     * @param color_attachment_count count of given color attachments for this pipeline
     */
    BlendInfo(uint32_t color_attachment_count);

    /**
     * Enable logic function for all attachments.
     * @param function the logic function to use - VK_LOGIC_OP_***
     */
    BlendInfo& enableLogicFunction(VkLogicOp function);

    /**
     * Disable logic function.
     */
    BlendInfo& disableLogicFunction();

    /**
     * Set a blend constant for all attachments.
     * @param r the   red component of the color
     * @param g the green component of the color
     * @param b the  blue component of the color
     * @param a the alpha component of the color
     */
    BlendInfo& setBlendConstant(float r, float g, float b, float a);
      
    /**
     * Get blending settings for given attachment.
     * @param color_attachment_index index of the attachment whose blending settings we modify.
     */
    BlendState getBlendSettings(int color_attachment_index);
     
    /**
     * Return a pointer to info object.
     */
    const VkPipelineColorBlendStateCreateInfo* getInfo() const;
};





#endif