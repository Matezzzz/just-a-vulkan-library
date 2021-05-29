#include "depth_stencil_info.h"

//shorthand for 'return *this;'
#define RTRN return *this;

DepthStencilInfo::DepthStencilInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr, 0,
        false, false, VK_COMPARE_OP_LESS, false, false,
        VkStencilOpState{}, VkStencilOpState{}, 0.f, 1.f}
{}
DepthStencilInfo& DepthStencilInfo::enableDepthTest()
{
    m_info.depthTestEnable = true; RTRN
}
DepthStencilInfo& DepthStencilInfo::disableDepthTest()
{
    m_info.depthBoundsTestEnable = false; RTRN
}
DepthStencilInfo& DepthStencilInfo::enableDepthWrite()
{
    m_info.depthWriteEnable = true; RTRN
}
DepthStencilInfo& DepthStencilInfo::disableDepthWrite()
{
    m_info.depthWriteEnable = false; RTRN
}
DepthStencilInfo& DepthStencilInfo::setDepthCompare(VkCompareOp compare)
{
    m_info.depthCompareOp = compare; RTRN
}
DepthStencilInfo& DepthStencilInfo::enableBoundDepthTest(float min_depth, float max_depth)
{
    m_info.depthBoundsTestEnable = true;
    m_info.minDepthBounds = min_depth;
    m_info.maxDepthBounds = max_depth;
    RTRN
}
DepthStencilInfo& DepthStencilInfo::disableBoundDepthTest()
{
    m_info.depthBoundsTestEnable = false; RTRN
}
DepthStencilInfo& DepthStencilInfo::enableStencilTest(const VkStencilOpState& front, const VkStencilOpState& back)
{
    m_info.stencilTestEnable = true;
    m_info.front = front;
    m_info.back = back;
    RTRN
}
DepthStencilInfo& DepthStencilInfo::disableStencilTest()
{
    m_info.stencilTestEnable = false; RTRN
}
const VkPipelineDepthStencilStateCreateInfo* DepthStencilInfo::getInfo() const
{
    //if any depth feature is enabled, return pointer to info, otherwise return nullptr
    if (m_info.depthTestEnable || m_info.depthBoundsTestEnable || m_info.stencilTestEnable){
        return &m_info;
    }
    return nullptr;
}


#undef RTRN