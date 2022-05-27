#include "flow_sections.h"




FlowSection::FlowSection(FlowContext& context) : m_context(context)
{}
void FlowSection::complete(){}

void FlowSection::transition(CommandBuffer&){}

void FlowSection::run(CommandBuffer& command_buffer){
    transition(command_buffer);
    execute(command_buffer);
}
void FlowSection::transitionDescriptors(CommandBuffer& buffer, const vector<FlowSectionDescriptorUsage>& descriptors_used){
    //go through all descriptors
    for (const FlowSectionDescriptorUsage& d : descriptors_used){
        int i = d.descriptor_index;
        //if current descriptor is an image
        if (d.isImage()){
            //get the state the image was previously in
            PipelineImageState& state = m_context.getImageState(i);
            //create image memory barrier on command buffer, specify: last stages during which the image was used, stages during which the image will be used, image memory barrier
            if (!state.barrier_done || state.layout != d.state.image.layout || state.access != d.state.image.access){
                //                                                                         state image was in before, new_state
                buffer.cmdBarrier(state.last_use, d.usage_stages, m_context.getImage(i).createMemoryBarrier(state, d.state.image));
                //update current image state to the one after barrier
                state = d.toImageState();
            }else{
                state.last_use = d.usage_stages;
            }
            state.barrier_done = false;
        //descriptor is a buffer
        }else{
            //get previous state of buffer
            PipelineBufferState& state = m_context.getBufferState(i);
            //create buffer memory barrier on command buffer, specify last stages when the buffer was used, and the ones it will be used in. Then create buffer memory barrier from previous access and required access
            buffer.cmdBarrier(state.last_use, d.usage_stages, m_context.getBuffer(i).createMemoryBarrier(state.access, d.state.buffer.access));
            //update current buffer state
            state = d.toBufferState();
        }
    }
}

FlowSection::~FlowSection(){}



FlowTransitionSection::FlowTransitionSection(FlowContext& ctx, const FlowSectionDescriptorUsage& usage) : FlowSection(ctx), m_descriptors_used{usage}
{}
FlowTransitionSection::FlowTransitionSection(FlowContext& ctx, const vector<FlowSectionDescriptorUsage>& usages) : FlowSection(ctx), m_descriptors_used{usages}
{}
void FlowTransitionSection::updateDescriptor(int index, int new_descriptor_index){
    m_descriptors_used[index].descriptor_index = new_descriptor_index;
}
void FlowTransitionSection::transition(CommandBuffer& buffer){
    transitionDescriptors(buffer, m_descriptors_used);
}
void FlowTransitionSection::execute(CommandBuffer&){}





FlowClearColorSection::FlowClearColorSection(FlowContext& ctx, int descriptor_index, VkClearValue clear_value) :
    //make the section transition the image to IMAGE_TRANSFER_DST - required state for clearing it with a color
    FlowTransitionSection(ctx, FlowSectionDescriptorUsage(descriptor_index, VK_PIPELINE_STAGE_TRANSFER_BIT, ImgState::TransferDst)), m_clear_value(clear_value), m_image(ctx.getImage(descriptor_index))
{}
void FlowClearColorSection::execute(CommandBuffer& buffer){
    //clear image with given color. current image state is IMAGE_TRANSFER_DST - it was transitioned using the section transition method 
    buffer.cmdClearColor(m_image, ImgState::TransferDst, m_clear_value.color);
}












FlowSectionListBase::FlowSectionListBase()
{}
FlowSectionListBase::FlowSectionListBase(const vector<FlowSection*>& sections){
    //reserve space for all subsections
    reserve(sections.size());
    //call addSection function to add all given sections to m_sections vector
    addSections(sections);
}
void FlowSectionListBase::completeAll(){
    for (auto& x : *this){
        x->complete();
    }
}
void FlowSectionListBase::transitionAll(CommandBuffer& buffer){
    for (auto& x : *this){
        x->transition(buffer);
    }
}
void FlowSectionListBase::executeAll(CommandBuffer& buffer){
    for (auto& x : *this){
        x->execute(buffer);
    }
}
void FlowSectionListBase::runAll(CommandBuffer& buffer){
    for (auto& x : *this){
        x->run(buffer);
    }
}
void FlowSectionListBase::addSections(){}

void FlowSectionListBase::addSections(const vector<FlowSection*>& sections){
    for (FlowSection* s : sections){
        emplace_back(unique_ptr<FlowSection>(s));
    }
}


FlowSectionList::FlowSectionList(FlowContext& ctx, const vector<FlowSection*>& sections) : FlowSection(ctx), FlowSectionListBase(sections)
{}
void FlowSectionList::complete(){
    completeAll();
}
void FlowSectionList::execute(CommandBuffer& buffer){
    runAll(buffer);
}

