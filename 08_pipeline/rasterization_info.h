#ifndef RASTERIZATION_INFO_H
#define RASTERIZATION_INFO_H

#include "../00_base/vulkan_base.h"

class RasterizationInfo
{
    VkPipelineRasterizationStateCreateInfo m_info;
public:
    /**
     * Create rasterization info for pipeline creation. 
     * Default parameters are: Depth clamp off, discard disabled, polygon mode fill, cull mode none, front face counter-clockwise, depth bias disabled, line width 1.f
     */
    RasterizationInfo();

    /**
     * Enable depth clamping. Needs feature depthClampEnable. 
     */ 
    RasterizationInfo& enableClampDepth();

    /**
     * Disable depth clamping.
     */
    RasterizationInfo& disableClampDepth();

    /**
     * If enabled, no fragments will be created, and pipeline will end before rasterization.
     */
    RasterizationInfo& enableDiscard();

    /**
     * Create fragments normally.
     */
    RasterizationInfo& disableDiscard();

    /**
     * All triangles will be filled with fragments, created by a fragment shader.
     */
    RasterizationInfo& setPolygonModeFill();

    /**
     * Triangles are only composed of border lines. Needs feature fillModeNonSolid.
     */
    RasterizationInfo& setPolygonModeLines();

    /**
     * Only points are drawn instead of lines and triangles. Needs feature fillModeNonSolid
     */
    RasterizationInfo& setPolygonModePoints();

    /**
     * Sets culling - which triangles are discarded before rasterization
     * @param flags which triangles to discard. Possible values - VK_CULL_MODE_***, FRONT_BIT, BACK_BIT, FRONT_AND_BACK
     */
    RasterizationInfo& setCulling(VkCullModeFlags flags);

    /**
     * Set face order to counter-clockwise, important for determining front and back faces for culling.
     */
    RasterizationInfo& setFaceOrderCounterclockwise();

    /**
     * Set face order to clockwise,  important for determining front and back faces for culling.
     */
    RasterizationInfo& setFaceOrderClockwise();

    /**
     * Enable bias for depth evaluation.
     * @param const_offset offset all depths by this value
     * @param bias_clamp maximum depth bias of a fragment
     * @param slope_factor scalar factor applied to fragment slope in bias calculations
     */
    RasterizationInfo& enableDepthBias(float const_offset, float bias_clamp, float slope_factor);
    
    /**
     * Disable bias for depth evaluation.
     */
    RasterizationInfo& disableDepthBias();

    /**
     * Set width of rasterized lines. Should it be greater than 1.f, the wideLines feature is needed.
     */
    RasterizationInfo& setLineWidth(float line_width);

    /**
     * Return a pointer to vulkan info object
     */
    const VkPipelineRasterizationStateCreateInfo* getInfo() const;
};


#endif