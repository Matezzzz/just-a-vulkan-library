#include "blend_info.h"


//shorthand for 'return *this;'
#define RTRN return *this;


BlendState::BlendState(VkPipelineColorBlendAttachmentState& blend_options) : 
    m_blend_options(blend_options)
{}
BlendState& BlendState::enableBlending()
{
    m_blend_options.blendEnable = true; RTRN
}
BlendState& BlendState::disableBlending()
{
    m_blend_options.blendEnable = false; RTRN
}
BlendState& BlendState::setColorBlend(VkBlendFactor source_factor, VkBlendFactor dist_factor, VkBlendOp blend_op)
{
    m_blend_options.srcColorBlendFactor = source_factor;
    m_blend_options.dstColorBlendFactor = dist_factor;
    m_blend_options.colorBlendOp = blend_op;
    RTRN
}
BlendState& BlendState::setAlphaBlend(VkBlendFactor source_factor, VkBlendFactor dist_factor, VkBlendOp blend_op)
{
    m_blend_options.srcAlphaBlendFactor = source_factor;
    m_blend_options.dstAlphaBlendFactor = dist_factor;
    m_blend_options.alphaBlendOp = blend_op;
    RTRN
}
BlendState& BlendState::setWriteMask(VkColorComponentFlags mask)
{
    m_blend_options.colorWriteMask = mask; RTRN
}





BlendInfo::BlendInfo(uint32_t color_attachment_count) : m_blend_settings(color_attachment_count, 
{false, VK_BLEND_FACTOR_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, VK_BLEND_OP_ADD,
        VK_BLEND_FACTOR_ONE, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD,
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT}),
    m_info{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0,
        false, VK_LOGIC_OP_CLEAR, (uint32_t) m_blend_settings.size(), m_blend_settings.data(), 
        {0.f, 0.f, 0.f, 0.f}}
{}

BlendInfo& BlendInfo::enableLogicFunction(VkLogicOp function)
{
    m_info.logicOpEnable = true;
    m_info.logicOp = function;
    RTRN
}
BlendInfo& BlendInfo::disableLogicFunction()
{
    m_info.logicOpEnable = false;
    RTRN
}
BlendInfo& BlendInfo::setBlendConstant(float r, float g, float b, float a)
{
    m_info.blendConstants[0] = r;
    m_info.blendConstants[1] = g;
    m_info.blendConstants[2] = b;
    m_info.blendConstants[3] = a;
    RTRN
}
BlendState BlendInfo::getBlendSettings(int color_attachment_index)
{
    //get reference to blend state of attachment with given index
    return BlendState(m_blend_settings[color_attachment_index]);
}
const VkPipelineColorBlendStateCreateInfo* BlendInfo::getInfo() const
{
    return &m_info;
}

#undef RTRN
