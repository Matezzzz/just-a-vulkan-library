#include "viewport_scissor_info.h"

//shorthand for 'return *this;'
#define RTRN return *this;

Viewport::Viewport(float width, float height) : m_viewport{0.f, 0.f, width, height, 0.f, 1.f}
{}
    
Viewport& Viewport::setSize(float width, float height)
{
    m_viewport.height = height;
    m_viewport.width = width;
    RTRN
}
Viewport& Viewport::setMinDepth(float min_depth)
{
    m_viewport.minDepth = min_depth; RTRN
}
Viewport& Viewport::setMaxDepth(float max_depth)
{
    m_viewport.maxDepth = max_depth; RTRN
}
Viewport& Viewport::setOffsets(float x, float y)
{
    m_viewport.x = x;
    m_viewport.y = y;
    RTRN
}
VkViewport& Viewport::get()
{
    return m_viewport;
}



Rect2D::Rect2D(uint32_t width, uint32_t height) : m_rect{{0, 0}, {width, height}}
{}
Rect2D& Rect2D::setSize(uint32_t width, uint32_t height)
{
    m_rect.extent = {width, height}; RTRN
}
Rect2D& Rect2D::setOffsets(int32_t x, int32_t y)
{
    m_rect.offset = {x, y}; RTRN
}
VkRect2D& Rect2D::get()
{
    return m_rect;
}



ViewportInfo::ViewportInfo(uint32_t width, uint32_t height) : m_viewport(static_cast<float>(width), static_cast<float>(height)), m_scissor_rect(width, height),
    m_info{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0,
        1, &m_viewport.get(), 1, &m_scissor_rect.get()}
{}

ViewportInfo& ViewportInfo::setSize(uint32_t width, uint32_t height){
    //set size of both viewport and scissor rect
    m_viewport.setSize(static_cast<float>(width), static_cast<float>(height));
    m_scissor_rect.setSize(width, height);
    RTRN
}
ViewportInfo& ViewportInfo::setOffsets(uint32_t x, uint32_t y){
    //set offsets of both viewport and scissor rect
    m_viewport.setOffsets(static_cast<float>(x), static_cast<float>(y));
    m_scissor_rect.setOffsets(x, y);
    RTRN
}
Viewport& ViewportInfo::getViewport(){
    return m_viewport;
}
Rect2D& ViewportInfo::getScissors(){
    return m_scissor_rect;
}
const VkPipelineViewportStateCreateInfo* ViewportInfo::getInfo() const{
    return &m_info;
}

