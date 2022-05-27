#include "flow_sections_pipeline.h"

FlowPipelineSectionDescriptors::FlowPipelineSectionDescriptors(const vector<FlowPipelineSectionDescriptorUsage>& images) : m_descriptors(images)
{}
FlowPipelineSectionDescriptors& FlowPipelineSectionDescriptors::add(const FlowPipelineSectionDescriptorUsage& usage){
    m_descriptors.push_back(usage);
    return *this;
}
FlowPipelineSectionDescriptors FlowPipelineSectionDescriptors::add(const FlowPipelineSectionDescriptorUsage& usage) const{
    FlowPipelineSectionDescriptors d = *this;
    return d.add(usage);
}
vector_ext<FlowSectionDescriptorUsage> FlowPipelineSectionDescriptors::getDescriptorUsages() const{
    //create a vector of usages, reserve one for each descriptor
    vector_ext<FlowSectionDescriptorUsage> usages;
    usages.reserve(m_descriptors.size());
    //go through all descriptors, add corresponding usage to vector for each one
    for (const FlowPipelineSectionDescriptorUsage& u : m_descriptors){
        usages.push_back(u.getUsage());
    }
    return usages;
}
vector<DescriptorUpdateInfo> FlowPipelineSectionDescriptors::getUpdateInfos(FlowContext& ctx) const{
    //reserve enough space for all update infos
    vector<DescriptorUpdateInfo> infos;
    infos.reserve(m_descriptors.size());
    //go through all descriptors, add corresponding update info for each one
    for (const FlowPipelineSectionDescriptorUsage& u : m_descriptors){
        infos.push_back(u.getUpdateInfo(ctx));
    }
    return infos;
}




//constructor for compute pipelines
FlowPipelineSection::FlowPipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const vector<FlowSectionDescriptorUsage>& usages, Pipeline&& pipeline) :
    FlowTransitionSection(flow_ctx, usages), m_pipeline_context(ctx), m_pipeline(pipeline)
{}
PipelineContext& FlowPipelineSection::getShaderContext(){
    return m_pipeline_context;
}



FlowSimplePipelineSection::FlowSimplePipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const FlowPipelineSectionDescriptors& usages, Pipeline&& pipeline) :
    FlowPipelineSection(flow_ctx, ctx, usages.getDescriptorUsages(), std::move(pipeline)), m_descriptor_update_infos(usages.getUpdateInfos(flow_ctx))
{
    //reserve one descriptor set for this section - this is needed so it can be allocated during complete() later
    if (m_descriptor_update_infos.size() != 0) m_pipeline_context.reserveDescriptorSets(1);
}
FlowSimplePipelineSection::FlowSimplePipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const FlowPipelineSectionDescriptors& usages,
        const vector_ext<FlowSectionDescriptorUsage>& usages_base, Pipeline&& pipeline) :
    FlowPipelineSection(flow_ctx, ctx, usages.getDescriptorUsages().add(usages_base), std::move(pipeline))
{
    //reserve one descriptor set for this section - this is needed so it can be allocated during complete() later
    if (m_descriptor_update_infos.size() != 0) m_pipeline_context.reserveDescriptorSets(1);
}
void FlowSimplePipelineSection::complete(){
    if (m_descriptor_update_infos.size() != 0){
        //allocate descriptor set and update all its' descriptors using update infos
        m_pipeline_context.allocateDescriptorSets(m_descriptor_set);
        m_descriptor_set.updateDescriptorsV(m_descriptor_update_infos);
    }
}
void FlowSimplePipelineSection::bind(CommandBuffer& buffer){
    if (m_descriptor_update_infos.size() != 0){
        //bind pipeline with descriptor set
        buffer.cmdBindPipeline(m_pipeline, m_descriptor_set);
    }else{
        buffer.cmdBindPipeline(m_pipeline);
    }
}
void FlowSimplePipelineSection::updateDescriptor(const DescriptorUpdateInfo& info, int new_descriptor_index){
    m_descriptor_set.updateDescriptors(info);
    FlowPipelineSection::updateDescriptor(m_descriptor_set.findDescriptor(info), new_descriptor_index);
}



FlowComputeSection::FlowComputeSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size) :
    FlowSimplePipelineSection(flow_ctx, ctx.getContext(name), usages, ctx.getContext(name).createComputePipeline()), m_compute_size(compute_size)
{}
void FlowComputeSection::execute(CommandBuffer& buffer){
    FlowSimplePipelineSection::bind(buffer);
    //dispatch given number of compute local groups
    buffer.cmdDispatchCompute(m_compute_size.x, m_compute_size.y, m_compute_size.z);
}








FlowGraphicsSection::FlowGraphicsSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
        uint32_t vertex_count, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
    FlowSimplePipelineSection(flow_ctx, ctx.getContext(name), usages, ctx.getContext(name).createPipeline(pipeline_info, render_pass, subpass_index)), m_vertex_count(vertex_count)
{}
FlowGraphicsSection::FlowGraphicsSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
        const vector_ext<FlowSectionDescriptorUsage>& usages_base, uint32_t vertex_count,
        const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
    FlowSimplePipelineSection(flow_ctx, ctx.getContext(name), usages, usages_base, ctx.getContext(name).createPipeline(pipeline_info, render_pass, subpass_index)), m_vertex_count(vertex_count)
{}
void FlowGraphicsSection::execute(CommandBuffer& buffer){
    FlowSimplePipelineSection::bind(buffer);
    //draw given number of vertices
    buffer.cmdDrawVertices(m_vertex_count);
}






FlowGraphicsVertexSection::FlowGraphicsVertexSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
    int vertex_buffer_descriptor_index, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
        FlowGraphicsSection(
            flow_ctx, ctx, name, usages,
            {FlowSectionDescriptorUsage{vertex_buffer_descriptor_index, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, BufState::Vertex}},
            flow_ctx.getVertexCount(vertex_buffer_descriptor_index), pipeline_info, render_pass, subpass_index
        ), m_vertex_buffer_index(vertex_buffer_descriptor_index)
{}
void FlowGraphicsVertexSection::execute(CommandBuffer& buffer){
    buffer.cmdBindVertexBuffer(m_context.getBuffer(m_vertex_buffer_index));
    FlowGraphicsSection::execute(buffer);
}





FlowComputeLoopPushConstantSection::FlowComputeLoopPushConstantSection(FlowContext& ctx, DirectoryPipelinesContext& shader_ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size, uint32_t iteration_count) :
    FlowComputePushConstantSection(ctx, shader_ctx, name, usages, compute_size), m_iteration_count(iteration_count)
{}
void FlowComputeLoopPushConstantSection::execute(CommandBuffer& buffer){
    //true during the first pass through the loop, false the rest of the time
    bool first = true;
    //go through all iterations
    for (uint32_t i = 0; i < m_iteration_count; i++){
        //no transition is needed the first time - descriptors were already transitioned when this section started
        if (first){
            first = false;
        }else{
            this->transition(buffer);
        }
        //write is_even_iteration push constant
        this->getPushConstantData().write("is_even_iteration", (i % 2 == 0) ? 1U : 0U);
        //execute T
        FlowComputePushConstantSection::execute(buffer);
    }
}