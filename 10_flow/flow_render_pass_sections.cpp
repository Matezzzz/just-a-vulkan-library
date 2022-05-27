#include "flow_render_pass_sections.h"



void FlowSectionListRenderPass::complete(){
    completeAll();
}
void FlowSectionListRenderPass::transition(CommandBuffer& buffer){
    transitionAll(buffer);
}
void FlowSectionListRenderPass::execute(CommandBuffer& buffer){
    executeAll(buffer);
}



FlowRenderPassAttachment::FlowRenderPassAttachment(const RenderPassAttachmentTypeClass& type_, int descriptor_index_, ImageState final_state_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_, ClearValue clear_value_) :
    type(type_), descriptor_index{descriptor_index_}, final_state{final_state_}, store_op{store_op_}, load_op{load_op_}, clear_value{clear_value_}
{}

vector<VkImageView> FlowRenderPassAttachments::getImages(FlowContext& ctx) const{
    return convert(convertImage, ctx);
}
vector<FlowSectionDescriptorUsage> FlowRenderPassAttachments::getUsages() const{
    return convert(convertUsage);
}
vector<ClearValue> FlowRenderPassAttachments::getClearValues() const{
    return convert(convertColor);
}
vector<FlowRenderPassAttachments::EndState> FlowRenderPassAttachments::getEndImageStates() const{
    return convert(getEndImageState);
}
ImageView FlowRenderPassAttachments::getDepthAttachment(FlowContext& ctx) const{
    for (const FlowRenderPassAttachment& a : (*this)){
        if (a.type.isDepth()) return ctx.getImage(a.descriptor_index);
    }
    return ImageView();
}
VkImageView FlowRenderPassAttachments::convertImage(const FlowRenderPassAttachment& a, FlowContext& ctx){
    return ctx.getImage(a.descriptor_index);
}
FlowSectionDescriptorUsage FlowRenderPassAttachments::convertUsage(const FlowRenderPassAttachment& a){
    return FlowSectionDescriptorUsage(a.descriptor_index, a.type.pipeline_first_use, a.type.pipeline_image_state);
}
ClearValue FlowRenderPassAttachments::convertColor(const FlowRenderPassAttachment& a){
    return a.clear_value;
}
FlowRenderPassAttachments::EndState FlowRenderPassAttachments::getEndImageState(const FlowRenderPassAttachment& a){
    return {a.descriptor_index, a.final_state, a.type.pipeline_last_use};
}



FlowSimpleRenderPassInfo::FlowSimpleRenderPassInfo(FlowContext& ctx, const FlowRenderPassAttachments& attachments){
    for (const FlowRenderPassAttachment& attachment : attachments){
        addAttachment(ctx.getImage(attachment.descriptor_index).getFormat(), attachment.type, attachment.final_state.layout, attachment.load_op, attachment.store_op);
    }
}




FlowFramebuffer::FlowFramebuffer(VkFramebuffer framebuffer_, const FlowRenderPassAttachments& attachments) :
    framebuffer{framebuffer_}, descriptors_used(attachments.getUsages()), end_states{attachments.getEndImageStates()}
{}
FlowFramebuffer& FlowFramebuffer::setFramebuffer(VkFramebuffer fm){
    framebuffer = fm; return *this;
}


FlowRenderPass::StandaloneImageAttachment::StandaloneImageAttachment(VkFormat format_, VkImageLayout final_layout_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_, ClearValue clear_value_) :
    format{format_}, final_layout{final_layout_}, load_op{load_op_}, store_op{store_op_}, clear_value{clear_value_}
{}



FlowRenderPass::FlowRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments) : 
    FlowSectionListRenderPass(ctx, attachments.getUsages()), m_settings{size, attachments.getClearValues()}
{
    render_pass = FlowSimpleRenderPassInfo(ctx, attachments).create();
}
FlowRenderPass::FlowRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments, const vector<StandaloneImageAttachment>& standalone_images) :
    FlowRenderPass(ctx, size, attachments)
{
    auto info = FlowSimpleRenderPassInfo(ctx, attachments);
    
    for (const StandaloneImageAttachment& img : standalone_images){
        info.addColorAttachment(img.format, img.final_layout, img.load_op, img.store_op);
        m_settings.addClearValue(img.clear_value);
    }
    render_pass = info.create();
}
void FlowRenderPass::executeRenderPass(CommandBuffer& buffer, const FlowFramebuffer& framebuffer){
    //transition all attachments used by the framebuffer
    transitionDescriptors(buffer, framebuffer.descriptors_used);
    //begin a render pass, execute all subsections, then end it
    buffer.cmdBeginRenderPass(m_settings, render_pass, framebuffer.framebuffer);
    FlowSectionListRenderPass::execute(buffer);
    buffer.cmdEndRenderPass();
    //by ending a render pass, all attachments changed their state - update the states in FlowContext as well
    for (const FlowRenderPassAttachments::EndState& state : framebuffer.end_states){
        m_context.getImageState(state.descriptor_index) = PipelineImageState{state.state, state.last_stage, true};
    }
}
//create a framebuffer using given attachments
VkFramebuffer FlowRenderPass::createFramebuffer(const glm::uvec2& size, const FlowRenderPassAttachments& attachments){
    return FramebufferInfo(size, attachments.getImages(m_context), render_pass).create();
}






    
FlowFramebufferRenderPass::FlowFramebufferRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments) :
    FlowRenderPass(ctx, size, attachments), m_framebuffer{createFramebuffer(size, attachments), attachments}
{}
void FlowFramebufferRenderPass::execute(CommandBuffer& command_buffer){
    executeRenderPass(command_buffer, m_framebuffer);
}





FlowDoubleFramebufferRenderPass::FlowDoubleFramebufferRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments_1, const FlowRenderPassAttachments& attachments_2) :
    FlowRenderPass(ctx, size, attachments_1), m_framebuffer_1(createFramebuffer(size, attachments_1), attachments_1), m_framebuffer_2(createFramebuffer(size, attachments_2), attachments_2)
{}
void FlowDoubleFramebufferRenderPass::swapFramebuffers(bool use_first){
    m_first_framebuffer_bound = use_first;
}
void FlowDoubleFramebufferRenderPass::execute(CommandBuffer& buffer){
    executeRenderPass(buffer, m_first_framebuffer_bound ? m_framebuffer_1 : m_framebuffer_2);
}




FlowSwapchainRenderPass::FlowSwapchainRenderPass(FlowContext& ctx, Swapchain& swapchain, const FlowRenderPassAttachments& additional_attachments) :
    FlowRenderPass(ctx, swapchain.getSize(), additional_attachments, 
        {StandaloneImageAttachment(swapchain.getFormat(), VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, ClearValue{0, 0, 0, 0})}
    ), m_swapchain{swapchain},
        m_framebuffer_settings(VK_NULL_HANDLE, additional_attachments)
{
    swapchain.createFramebuffers(render_pass, additional_attachments.getDepthAttachment(ctx));
}
void FlowSwapchainRenderPass::execute(CommandBuffer& command_buffer){
    acquireImage();
    //transition window image to be rendered into
    command_buffer.cmdBarrier(VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        m_current_image.createMemoryBarrier(ImgState::Undefined, ImgState::ColorAttachment)
    );
    executeRenderPass(command_buffer, m_framebuffer_settings.setFramebuffer(m_current_image.getFramebuffer()));
}
void FlowSwapchainRenderPass::waitForSwapchainImage(){
    //wait until window image can be rendered into
    m_swapchain.prepareToDraw();
}
void FlowSwapchainRenderPass::presentResult(Queue& queue){
    m_swapchain.presentImage(m_current_image, queue);
}
void FlowSwapchainRenderPass::acquireImage(){
    //get current window image to render into
    m_current_image = m_swapchain.acquireImage();
}