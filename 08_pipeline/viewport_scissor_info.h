#ifndef VIEWPORT_SCISSOR_INFO_H
#define VIEWPORT_SCISSOR_INFO_H

#include "../00_base/vulkan_base.h"


class Viewport
{
    VkViewport m_viewport;
public:
    /**
     * Create a viewport with given size, 0 offset, 0.0 minimum depth and 1.0 maximum depth.
     * @param width the width of the viewport(pixels)
     * @param height the height of the viewport(pixels)
     */
    Viewport(float width, float height);
    
    /**
     * Set viewport size.
     * @param width new viewport width(piels)
     * @param height new viewport height(pixels)
     */
    Viewport& setSize(float width, float height);

    /**
     * Set minimum depth value for the Viewport. All fragments outside won't be drawn.
     * @param min_depth new minimum depth value(between 0.0 and 1.0)
     */
    Viewport& setMinDepth(float min_depth);

    /**
     * Set maximum depth value for the Viewport. All fragments with bigger depth won't be drawn.
     * @param max_depth new maximum depth value(between 0.0 and 1.0)
     */
    Viewport& setMaxDepth(float max_depth);

    /**
     * Set x and y offsets for the viewport.
     * @param x new x offset(pixels)
     * @param y new y offset(pixels)
     */
    Viewport& setOffsets(float x, float y);

    /**
     * Return vulkan viewport object as a reference
     * @return vulkan Viewport reference
     */
    VkViewport& get();
};


class Rect2D
{
    VkRect2D m_rect;
public:
    /**
     * Create a new 2D rectangle(used for scissor tests) with given width and height
     * @param width rectangle width(pixels)
     * @param height rectangle height(pixels)
     */
    Rect2D(uint32_t width, uint32_t height);

    /**
     * Set new rectangle size
     * @param width new rectangle width(pixels)
     * @param height new rectangle height(pixels)
     */
    Rect2D& setSize(uint32_t width, uint32_t height);

    /**
     * Set new rectangle offsets
     * @param x new rectangle x offset
     * @param y new rectangle y offset
     */
    Rect2D& setOffsets(int32_t x, int32_t y);

    /**
     * Return vulkan Rect2D object as a reference
     * @return vulkan Rect2D reference
     */
    VkRect2D& get();
};


class ViewportInfo
{
    Viewport m_viewport;
    Rect2D m_scissor_rect;
    VkPipelineViewportStateCreateInfo m_info;
public:
    /**
     * Create viewport info for pipeline creation with given width and height
     * @param width the width of the viewport and scissor rectangle(pixels)
     * @param height the height of the viewport and scissor rectangle(pixels)
     */
    ViewportInfo(uint32_t width, uint32_t height);

    /**
     * Set both viewport and scissor rectangle size.
     * @param width new width of viewport and scissor rectangle(pixels)
     * @param height new height of viewport and scissor rectangle(pixels)
     */
    ViewportInfo& setSize(uint32_t width, uint32_t height);

    /**
     * Set both viewport and scissor offsets.
     * @param x new x offset of viewport and scissor rectangle(pixels)
     * @param y new y offset of viewport and scissor rectangle(pixels)
     */
    ViewportInfo& setOffsets(uint32_t x, uint32_t y);
    
    /**
     * Return a reference to the viewport object
     * @return A reference to viewport object.
     */
    Viewport& getViewport();

    /**
     * Return a reference to the scissor rectangle object
     * @return A reference to scissor rectangle object.
     */
    Rect2D& getScissors();

    /**
     * Return a pointer to info object.
     */
    const VkPipelineViewportStateCreateInfo* getInfo() const;
};


#endif