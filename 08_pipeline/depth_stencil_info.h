#ifndef DEPTH_STENCIL_INFO_H
#define DEPTH_STENCIL_INFO_H

#include "../00_base/vulkan_base.h"


class DepthStencilInfo
{
    VkPipelineDepthStencilStateCreateInfo m_info;
public:
    /**
     * Creates a new depth and stencil info for a pipeline.
     * Default parameters are: No depth test, depth write on, depth compare ALWAYS, depth bounds test off, stencil test off
     */
    DepthStencilInfo();

    /**
     * Enable depth test, hiding all fragments with bigger depth than the ones in front. For this, depth buffer is needed.
     */
    DepthStencilInfo& enableDepthTest();

    /**
     * Disable depth test, depth buffer not needed.
     */
    DepthStencilInfo& disableDepthTest();

    /**
     * Update depth buffer values when fragments are drawn.
     */
    DepthStencilInfo& enableDepthWrite();

    /**
     * Don't modify depth buffer.
     */
    DepthStencilInfo& disableDepthWrite();

    /**
     * Set depth comparison function. 
     * @param compare Possible values - VK_COMPARE_OP_*** - NEVER, LESS, EQUAL, LESS_OR_EQUAL, GREATER, NOT_EQUAL, GREATER_OR_EQUAL, ALWAYS
     */
    DepthStencilInfo& setDepthCompare(VkCompareOp compare);

    /**
     * Enable depth bounds - fragments with depths outside given range are discarded.
     * @param min_depth the minimum depth value for the fragment to pass, between 0.0 and 1.0 inclusive
     * @param max_depth the maximum depth value for the fragment to pass, between 0.0 and 1.0 inclusive
     */
    DepthStencilInfo& enableBoundDepthTest(float min_depth, float max_depth);

    /**
     * Disable depth bounds - no bounds test is done.
     */
    DepthStencilInfo& disableBoundDepthTest();

    /**
     * Enable stencil test - a simple integer operation for every fragment, which can cause the fragment to be discarded. A stencil buffer is needed.
     * @param front configures stencil test for fragments in the direction of camera
     * @param back configures stencil test for fragments with their back in the direction of camera
     */
    DepthStencilInfo& enableStencilTest(const VkStencilOpState& front, const VkStencilOpState& back);
    
    /**
     * Disable stencil test. No stencil buffer is needed.
     */
    DepthStencilInfo& disableStencilTest();

    /**
     * Return a pointer to info object.
     */
    const VkPipelineDepthStencilStateCreateInfo* getInfo() const;
};

#endif