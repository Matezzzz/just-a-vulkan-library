#include "pipeline.h"
#include "../01_device/allocator.h"


Pipeline::Pipeline(VkPipeline pipeline, VkPipelineLayout layout, VkPipelineBindPoint bind_point) :
    m_pipeline(pipeline), m_layout(layout), m_bind_point(bind_point)
{}
Pipeline::Pipeline() : Pipeline(VK_NULL_HANDLE, VK_NULL_HANDLE, VK_PIPELINE_BIND_POINT_MAX_ENUM)
{}
VkPipelineLayout Pipeline::getLayout() const{
    return m_layout;
}
Pipeline::operator VkPipeline() const{
    return m_pipeline;
}
VkPipeline Pipeline::get() const{
    return m_pipeline;
}
VkPipelineBindPoint Pipeline::getBindPoint() const{
    return m_bind_point;
}



PipelineInfo::PipelineInfo(uint32_t width, uint32_t height, uint32_t color_attachment_count) : 
    m_viewport_info(width, height), m_blend_info(color_attachment_count)
{}
PipelineInfo::PipelineInfo(const glm::uvec2& size, uint32_t color_attachment_count) : 
    PipelineInfo(size.x, size.y, color_attachment_count)
{}



VkPipeline PipelineInfo::create(const vector<VkPipelineShaderStageCreateInfo>& shader_modules, const VkPipelineLayout& layout, VkRenderPass render_pass, uint32_t subpass_index) const
{
    //fill info structure with all creation infos
    VkGraphicsPipelineCreateInfo info{
        //                                                        flags
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0,
        static_cast<uint32_t>(shader_modules.size()), shader_modules.data(),
        m_vertex_input_info.getInfo(), m_assembly_info.getInfo(),
        m_tesselation_info.getInfo(), m_viewport_info.getInfo(),
        m_rasterization_info.getInfo(), m_multisample_info.getInfo(),
        m_depth_stencil_info.getInfo(), m_blend_info.getInfo(),
        m_dynamic_info.getInfo(), layout,
        render_pass, subpass_index,
        //base pipeline handle, base pipeline index; These are used only when deriving pipelines
        VK_NULL_HANDLE, 0
    };
    return g_allocator.get().createPipeline(info);
}
//functions returning references to modify each of info objects
VertexInputInfo& PipelineInfo::getVertexInputInfo() {return m_vertex_input_info;}
AssemblyInfo& PipelineInfo::getAssemblyInfo() {return m_assembly_info;}
TesselationInfo& PipelineInfo::getTesselationInfo() {return m_tesselation_info;}
ViewportInfo& PipelineInfo::getViewportInfo() {return m_viewport_info;}
RasterizationInfo& PipelineInfo::getRasterizationInfo() {return m_rasterization_info;}
MultisampleInfo& PipelineInfo::getMultisampleInfo() {return m_multisample_info;}
DepthStencilInfo& PipelineInfo::getDepthStencilInfo(){return m_depth_stencil_info;}
BlendInfo& PipelineInfo::getBlendInfo(){return m_blend_info;}
DynamicInfo& PipelineInfo::getDynamicInfo(){return m_dynamic_info;}


ComputePipelineInfo::ComputePipelineInfo()
{}
VkPipeline ComputePipelineInfo::create(const VkPipelineShaderStageCreateInfo& shader_module, const VkPipelineLayout& layout) const{
    //fill structure for creating a compute pipeline
    VkComputePipelineCreateInfo info{
        //                                                       flags
        VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, nullptr, 0,
        shader_module, layout,
        //base pipeline handle, base pipeline index; These are used only when deriving pipelines
        VK_NULL_HANDLE, 0
    };
    return g_allocator.get().createPipeline(info);
}
