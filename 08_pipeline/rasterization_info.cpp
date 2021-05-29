#include "rasterization_info.h"

//shorthand for 'return *this;'
#define RTRN return *this;

RasterizationInfo::RasterizationInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0,
        false, false, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE,
        false, 0.f, 0.f, 1.f, 1.f}
{}
RasterizationInfo& RasterizationInfo::enableClampDepth()
{
    m_info.depthClampEnable = true; RTRN
}
RasterizationInfo& RasterizationInfo::disableClampDepth()
{
    m_info.depthClampEnable = false; RTRN
}
RasterizationInfo& RasterizationInfo::enableDiscard()
{
    m_info.rasterizerDiscardEnable = true; RTRN
}
RasterizationInfo& RasterizationInfo::disableDiscard()
{
    m_info.rasterizerDiscardEnable = false; RTRN
}
RasterizationInfo& RasterizationInfo::setPolygonModeFill()
{
    m_info.polygonMode = VK_POLYGON_MODE_FILL; RTRN
}
RasterizationInfo& RasterizationInfo::setPolygonModeLines()
{
    m_info.polygonMode = VK_POLYGON_MODE_LINE; RTRN
}
RasterizationInfo& RasterizationInfo::setPolygonModePoints()
{
    m_info.polygonMode = VK_POLYGON_MODE_POINT; RTRN
}
RasterizationInfo& RasterizationInfo::setCulling(VkCullModeFlags culling)
{
    m_info.cullMode = culling; RTRN
}
RasterizationInfo& RasterizationInfo::setFaceOrderCounterclockwise()
{
    m_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; RTRN
}
RasterizationInfo& RasterizationInfo::setFaceOrderClockwise()
{
    m_info.frontFace = VK_FRONT_FACE_CLOCKWISE; RTRN
}
RasterizationInfo& RasterizationInfo::enableDepthBias(float const_offset, float bias_clamp, float slope_factor)
{
    m_info.depthBiasEnable = true;
    m_info.depthBiasConstantFactor = const_offset;
    m_info.depthBiasClamp = bias_clamp;
    m_info.depthBiasSlopeFactor = slope_factor;
    RTRN
}
RasterizationInfo& RasterizationInfo::disableDepthBias()
{
    m_info.depthBiasEnable = false; RTRN
}
RasterizationInfo& RasterizationInfo::setLineWidth(float line_width)
{
    m_info.lineWidth = line_width; RTRN
}
const VkPipelineRasterizationStateCreateInfo* RasterizationInfo::getInfo() const
{
    return &m_info;   
}

#undef RTRN