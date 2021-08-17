#include "flow_sections.h"




FlowSection::FlowSection(const vector<FlowSectionDescriptorUsage>& usages) : m_descriptors_used(usages)
{}
void FlowSection::complete(){}

void FlowSection::transition(CommandBuffer& buffer, FlowDescriptorContext& flow_context){
    //go through all descriptors
    for (const FlowSectionDescriptorUsage& d : m_descriptors_used){
        int i = d.descriptor_index;
        //if current descriptor is an image
        if (d.isImage()){
            //get the state the image was previously in
            PipelineImageState& state = flow_context.getImageState(i);
            //create image memory barrier on command buffer, specify: last stages during which the image was used, stages during which the image will be used, image memory barrier
            //                                                                         state image was in before, new_state
            buffer.cmdBarrier(state.last_use, d.usage_stages, flow_context.getImage(i).createMemoryBarrier(state, d.state.image));
            //update current image state to the one after barrier
            state = d.toImageState();
        //descriptor is a buffer
        }else{
            //get previous state of buffer
            PipelineBufferState& state = flow_context.getBufferState(i);
            //create buffer memory barrier on command buffer, specify last stages when the buffer was used, and the ones it will be used in. Then create buffer memory barrier from previous access and required access
            buffer.cmdBarrier(state.last_use, d.usage_stages, flow_context.getBuffer(i).createMemoryBarrier(state.access, d.state.buffer.access));
            //update current buffer state
            state = d.toBufferState();
        }
    }
}
void FlowSection::run(CommandBuffer& command_buffer, FlowDescriptorContext& flow_context){
    transition(command_buffer, flow_context);
    execute(command_buffer);
}


FlowTransitionSection::FlowTransitionSection(const FlowSectionDescriptorUsage& usage) : FlowSection({usage})
{}
FlowTransitionSection::FlowTransitionSection(const vector<FlowSectionDescriptorUsage>& usages) : FlowSection(usages)
{}
void FlowTransitionSection::execute(CommandBuffer&){}





FlowClearColorSection::FlowClearColorSection(FlowDescriptorContext& ctx, int descriptor_index, VkClearValue clear_value) :
    //make the section transition the image to IMAGE_TRANSFER_DST - required state for clearing it with a color
    FlowSection({FlowSectionDescriptorUsage(descriptor_index, VK_PIPELINE_STAGE_TRANSFER_BIT, ImageState{IMAGE_TRANSFER_DST})}), m_clear_value(clear_value), m_image(ctx.getImage(descriptor_index))
{}
void FlowClearColorSection::execute(CommandBuffer& buffer){
    //clear image with given color. current image state is IMAGE_TRANSFER_DST - it was transitioned using the section transition method 
    buffer.cmdClearColor(m_image, ImageState{IMAGE_TRANSFER_DST}, m_clear_value.color);
}



FlowPipelineSectionDescriptors::FlowPipelineSectionDescriptors(FlowDescriptorContext& ctx, const vector<FlowPipelineSectionDescriptorUsage>& images) : m_context(ctx), m_descriptors(images)
{}
vector<FlowSectionDescriptorUsage> FlowPipelineSectionDescriptors::getDescriptorUsages() const{
    //create a vector of usages, reserve one for each descriptor
    vector<FlowSectionDescriptorUsage> usages;
    usages.reserve(m_descriptors.size());
    //go through all descriptors, add corresponding usage to vector for each one
    for (const FlowPipelineSectionDescriptorUsage& u : m_descriptors){
        usages.push_back(u.getUsage());
    }
    return usages;
}
vector<DescriptorUpdateInfo> FlowPipelineSectionDescriptors::getUpdateInfos() const{
    //reserve enough space for all update infos
    vector<DescriptorUpdateInfo> infos;
    infos.reserve(m_descriptors.size());
    //go through all descriptors, add corresponding update info for each one
    for (const FlowPipelineSectionDescriptorUsage& u : m_descriptors){
        infos.push_back(u.getUpdateInfo(m_context));
    }
    return infos;
}




//constructor for compute pipelines
FlowPipelineSection::FlowPipelineSection(DirectoryPipelinesContext& ctx, const string& name, const vector<FlowSectionDescriptorUsage>& usages) :
    FlowSection(usages), m_context(ctx.getContext(name)), m_pipeline(m_context.createComputePipeline())
{}
//constructor for graphical pipelines
FlowPipelineSection::FlowPipelineSection(DirectoryPipelinesContext& ctx, const string& name, const vector<FlowSectionDescriptorUsage>& usages, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
    FlowSection(usages), m_context(ctx.getContext(name)), m_pipeline(m_context.createPipeline(pipeline_info, render_pass, subpass_index))
{}
PipelineContext& FlowPipelineSection::getShaderContext(){
    return m_context;
}



FlowSimplePipelineSection::FlowSimplePipelineSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages) :
    FlowPipelineSection(ctx, name, usages.getDescriptorUsages()), m_descriptor_update_infos(usages.getUpdateInfos())
{
    //reserve one descriptor set for this section - this is needed so it can be allocated during complete() later
    m_context.reserveDescriptorSets(1);
}
FlowSimplePipelineSection::FlowSimplePipelineSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
    FlowPipelineSection(ctx, name, usages.getDescriptorUsages(), pipeline_info, render_pass, subpass_index), m_descriptor_update_infos(usages.getUpdateInfos())
{
    //reserve one descriptor set for this section - this is needed so it can be allocated during complete() later
    m_context.reserveDescriptorSets(1);
}
void FlowSimplePipelineSection::complete(){
    //allocate descriptor set and update all its' descriptors using update infos
    m_context.allocateDescriptorSets(m_descriptor_set);
    m_descriptor_set.updateDescriptorsV(m_descriptor_update_infos);
}
void FlowSimplePipelineSection::bind(CommandBuffer& buffer){
    //bind pipeline with descriptor set
    buffer.cmdBindPipeline(m_pipeline, m_descriptor_set);
}



FlowComputeSection::FlowComputeSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size) :
    FlowSimplePipelineSection(ctx, name, usages), m_compute_size(compute_size)
{}
void FlowComputeSection::execute(CommandBuffer& buffer){
    FlowSimplePipelineSection::bind(buffer);
    //dispatch given number of compute local groups
    buffer.cmdDispatchCompute(m_compute_size.x, m_compute_size.y, m_compute_size.z);
}



FlowGraphicsSection::FlowGraphicsSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
        uint32_t vertex_count, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index) :
    FlowSimplePipelineSection(ctx, name, usages, pipeline_info, render_pass, subpass_index), m_vertex_count(vertex_count)
{}
void FlowGraphicsSection::execute(CommandBuffer& buffer){
    FlowSimplePipelineSection::bind(buffer);
    //draw given number of vertices
    buffer.cmdDrawVertices(m_vertex_count);
}







void FlowSectionList::complete(){
    for (unique_ptr<FlowSection>& s : m_sections){
        s->complete();
    }
}
void FlowSectionList::execute(CommandBuffer& buffer){
    for (unique_ptr<FlowSection>& s : m_sections){
        s->run(buffer, m_context);
    }
}

void FlowSectionList::addSections(){}

