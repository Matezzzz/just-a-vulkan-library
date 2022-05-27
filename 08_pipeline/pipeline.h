#ifndef PIPELINE_H
#define PIPELINE_H

#include "../00_base/vulkan_base.h"
#include "rasterization_info.h"
#include "depth_stencil_info.h"
#include "simple_infos.h"
#include "viewport_scissor_info.h"
#include "blend_info.h"
#include "multisample_info.h"
#include <glm/glm.hpp>


class Pipeline
{
    VkPipeline m_pipeline;
    VkPipelineLayout m_layout;
    VkPipelineBindPoint m_bind_point;
    //PushConstantManager m_push_constant_manager;
public:
    /**
     * Create a pipeline wrapper with given pipeline object and layout
     * @param pipeline the pipeline handle
     * @param layout the pipeline layout
     * @param bind_point whether pipeline is graphics or compute
     */
    Pipeline(VkPipeline pipeline, VkPipelineLayout layout, VkPipelineBindPoint bind_point);

    /**
     * Construct an empty, invalid pipeline
     */
    Pipeline();

    /**
     * Return a handle to pipeline layout.
     */
    VkPipelineLayout getLayout() const;

    /**
     * Converter to VkPipeline - return the pipeline handle.
     */
    operator VkPipeline() const;

    /**
     * Returns pipeline handle.
     */
    VkPipeline get() const;
    
    /**
     * Returns pipeline bind point - is either VK_PIPELINE_BIND_POINT_GRAPHICS or VK_PIPELINE_BIND_POINT_COMPUTE
     */
    VkPipelineBindPoint getBindPoint() const;
};



/**
 * PipelineInfo
 *  - Holds all data needed to create a pipeline.
 */
class PipelineInfo{
    VertexInputInfo m_vertex_input_info;
    AssemblyInfo m_assembly_info;
    TesselationInfo m_tesselation_info;
    ViewportInfo m_viewport_info;
    RasterizationInfo m_rasterization_info;
    MultisampleInfo m_multisample_info;
    DepthStencilInfo m_depth_stencil_info;
    BlendInfo m_blend_info;
    DynamicInfo m_dynamic_info;
public:
    /**
     * Create a new graphics pipeline info object with given shaders, viewport width and height, and given color attachment count.
     * Default parameters are as follows:
     *  - VertexInputInfo - no buffers active
     *  - AssemblyInfo - topology is triangle list, primitive restart is disabled
     *  - TesselationInfo - tesselation is disabled by default, is enabled by setting a number of tess points
     *  - ViewportInfo - both scissor and viewport rect are set to the window size
     *  - RasterizationInfo - depth clamp off, discard disabled, polygon mode fill, cull mode none, front face counter-clockwise, depth bias disabled, line width 1.f
     *  - MultisampleInfo - 1 sample per fragment, sample shading disabled, no shading mask, alpha to coverage disabled, alpha to one disabled
     *  - DepthStencilInfo - no depth test, depth write on, depth compare always, depth bounds test off, stencil test off
     *  - BlendInfo
     *    - Blend state for each layer set to disabled, source color factor alpha, target color factor (1-src_alpha), color blend op add, source alpha alpha, source target factor (1-src_alpha), alpha blend op add, all colors(RGBA) written
     *    - Logical operation disabled, logic operation clear, blend constants 0, 0, 0, 0\n
     *  - DynamicInfo - no dynamic states enabled 
     * @param width width of space to render to
     * @param height height of space to render to
     * @param color_attachment_count how many color attachments does the pipeline use. Needed for blending.
     */
    PipelineInfo(uint32_t width, uint32_t height, uint32_t color_attachment_count);

    /**
     * Create a new graphics pipeline info object with given shaders, viewport width and height, and given color attachment count.
     * Default parameters are as follows:
     *  - VertexInputInfo - no buffers active
     *  - AssemblyInfo - topology is triangle list, primitive restart is disabled
     *  - TesselationInfo - tesselation is disabled by default, is enabled by setting a number of tess points
     *  - ViewportInfo - both scissor and viewport rect are set to the window size
     *  - RasterizationInfo - depth clamp off, discard disabled, polygon mode fill, cull mode none, front face counter-clockwise, depth bias disabled, line width 1.f
     *  - MultisampleInfo - 1 sample per fragment, sample shading disabled, no shading mask, alpha to coverage disabled, alpha to one disabled
     *  - DepthStencilInfo - no depth test, depth write on, depth compare always, depth bounds test off, stencil test off
     *  - BlendInfo
     *    - Blend state for each layer set to disabled, source color factor alpha, target color factor (1-src_alpha), color blend op add, source alpha factor one, source target factor zero, alpha blend op add, all colors(RGBA) written
     *    - Logical operation disabled, logic operation clear, blend constants 0, 0, 0, 0\n
     *  - DynamicInfo - no dynamic states enabled 
     * @param size width, height of the space to render to
     * @param color_attachment_count how many color attachments does the pipeline use. Needed for blending.
     */
    PipelineInfo(const glm::uvec2& size, uint32_t color_attachment_count);

    /**
     * Create a pipeline object for the given render pass and subpass
     * @param shaders the shaders to use in the pipeline
     * @param layout the pipeline layout to use
     * @param render_pass the render pass for which the pipeline will be used
     * @param subpass_index the index of a subpass in the given render pass.
     */
    VkPipeline create(const vector<VkPipelineShaderStageCreateInfo>& shader_modules, const VkPipelineLayout& layout, VkRenderPass render_pass, uint32_t subpass_index = 0) const;

    /**
     * Return a reference to vertex input info. This defines the how buffer data should be used in shaders.
     */
    VertexInputInfo& getVertexInputInfo();

    /**
     * Return a reference to assembly info. This controls how primitives are assembled.
     */
    AssemblyInfo& getAssemblyInfo();

    /**
     * Return a reference to tesselation info. This controls tesselation stage parameters.
     */
    TesselationInfo& getTesselationInfo();

    /**
     * Return a reference to viewport info. This controls the viewport and scissor tests.
     */
    ViewportInfo& getViewportInfo();

    /**
     * Return a reference to rasterization info. This controls how rasterization is carried out.
     */
    RasterizationInfo& getRasterizationInfo();

    /**
     * Return a reference to multisample info. It is used to modify multisampling parameters.
     */
    MultisampleInfo& getMultisampleInfo();

    /**
     * Return a reference to depth and stencil info. This controls the behaviour of depth and stencil tests.
     */
    DepthStencilInfo& getDepthStencilInfo();

    /**
     * Return a reference to blend info. This manages logical operations and fragment blending settings.
     */
    BlendInfo& getBlendInfo();

    /**
     * Return a reference to dynamic info. This manages enabled dynamic pipeline states.
     */
    DynamicInfo& getDynamicInfo();
};



class ComputePipelineInfo{
public:
    ComputePipelineInfo();

    /**
     * Create the pipeline and return it's handle.
     */
    VkPipeline create(const VkPipelineShaderStageCreateInfo& shader_module, const VkPipelineLayout& layout) const;
};


#endif