#ifndef FLOW_RENDER_PASS_SECTIONS_H
#define FLOW_RENDER_PASS_SECTIONS_H

#include "../02_swapchain/swapchain.h"
#include "../06_render_passes/framebuffer.h"
#include "../06_render_passes/renderpass_specializations.h"
#include "../10_flow/flow_sections.h"


/**
 * @brief Holds any number of sections, transition() transitions all, execute() executes all
 * 
 */
class FlowSectionListRenderPass : public FlowTransitionSection, public FlowSectionListBase{
public:
    FlowSectionListRenderPass(FlowContext& ctx, const vector<FlowSectionDescriptorUsage> &usages) : FlowTransitionSection(ctx, usages)
    {}

    //complete all subsections
    virtual void complete();

    //transition all subsections
    virtual void transition(CommandBuffer& command_buffer);

    //execute all subsections
    virtual void execute(CommandBuffer& command_buffer);
};



/**
 * @brief Holds a single render pass attachment for a flow renderpass section
 * 
 */
class FlowRenderPassAttachment{
public:
    //type of this attachment - specifies default settings, layout to be in during the renderpass, and more
    const RenderPassAttachmentTypeClass& type;

    //index of the attachment image in associated FlowContext
    int descriptor_index;

    //state to transition the attachment into after render pass ends
    ImageState final_state;

    //store operation for this attachment - store/dont care
    VkAttachmentStoreOp store_op;

    //load operation for this attachment - load/clear/dont care
    VkAttachmentLoadOp load_op;

    //what value to clear this attachment with, if using clear load op
    ClearValue clear_value;
public:
    FlowRenderPassAttachment(const RenderPassAttachmentTypeClass& type_, int descriptor_index_, ImageState final_state_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_, ClearValue clear_value_);
};


/**
 * @brief Used to create a renderpass attachment without having to specify all the parameters - defaults from type will be used instead
 * 
 * @tparam type_ type of the attachment
 */
template<const RenderPassAttachmentTypeClass& type_>
class FlowSpecificRenderPassAttachment : public FlowRenderPassAttachment{
public:
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_) :
        FlowSpecificRenderPassAttachment(descriptor_index_, final_state_, type_.default_clear_value)
    {}
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_, VkAttachmentStoreOp store_op_) :
        FlowSpecificRenderPassAttachment(descriptor_index_, final_state_, type_.default_load_op, store_op_)
    {}
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_) :
        FlowSpecificRenderPassAttachment(descriptor_index_, final_state_, load_op_, store_op_, type_.default_clear_value)
    {}
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_, const ClearValue& clear_value_) :
        FlowSpecificRenderPassAttachment(descriptor_index_, final_state_, type_.default_store_op, clear_value_)
    {}
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_, VkAttachmentStoreOp store_op_, const ClearValue& clear_value_) :
        FlowSpecificRenderPassAttachment(descriptor_index_, final_state_, type_.default_load_op, store_op_, clear_value_)
    {}
    FlowSpecificRenderPassAttachment(int descriptor_index_, ImageState final_state_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_, const ClearValue& clear_value_) :
        FlowRenderPassAttachment(type_, descriptor_index_, final_state_, load_op_, store_op_, clear_value_)
    {}
};



using FlowColorAttachment = FlowSpecificRenderPassAttachment<RenderPassAttType::Color>;
using FlowDepthAttachment = FlowSpecificRenderPassAttachment<RenderPassAttType::Depth>;






class FlowRenderPassAttachments : public vector_ext<FlowRenderPassAttachment>{
public:
    //holds an end state for one attachment
    struct EndState{
        int descriptor_index;
        ImageState state;
        VkPipelineStageFlags last_stage;
    };

    /**
     * @brief Save all specified attachments as a vector
     * 
     * @tparam The attachments to save. Should all inherit from FlowRenderPassAttachment
     * @param a 
     */
    template<typename... Attachments>
    FlowRenderPassAttachments(Attachments... a) : vector_ext<FlowRenderPassAttachment>{a...}
    {}

    //take the image view of each attachment
    vector<VkImageView> getImages(FlowContext& ctx) const;

    //take the usage of each attachment
    vector<FlowSectionDescriptorUsage> getUsages() const;

    //take the clear value of each attachment
    vector<ClearValue> getClearValues() const;

    //get the RenderPassEndImageState for each attachment - contains descriptor index, final state, and last stage of usage
    vector<EndState> getEndImageStates() const;

    //get a depth attachment, or invalid image view if none is attached
    ImageView getDepthAttachment(FlowContext& ctx) const;
private:
    //get the image from flow context
    static VkImageView convertImage(const FlowRenderPassAttachment& a, FlowContext& ctx);

    //get just the usage from given attachment
    static FlowSectionDescriptorUsage convertUsage(const FlowRenderPassAttachment& a);

    //get just the color from given attachment
    static ClearValue convertColor(const FlowRenderPassAttachment& a);

    //get RenderPassEndImageState from given attachment - contains descriptor index, final state, and last stage of usage
    static EndState getEndImageState(const FlowRenderPassAttachment& a);
};


/**
 * @brief A render pass info consisting of one subpass, constructor takes FlowRenderPassAttachments class
 * 
 */
class FlowSimpleRenderPassInfo : public SimpleRenderPassInfo{
public:
    FlowSimpleRenderPassInfo(FlowContext& ctx, const FlowRenderPassAttachments& attachments);
};




/**
 * @brief Given a compatible renderpass, contains all info to run a render pass, and update descriptor states in flow context accordingly
 * 
 */
class FlowFramebuffer{
public:
    VkFramebuffer framebuffer;
    //when are the descriptors used during the render pass
    vector<FlowSectionDescriptorUsage> descriptors_used;
    //how will descriptor states change when render pass ends
    vector<FlowRenderPassAttachments::EndState> end_states;

    /**
     * @brief Create new flow framebuffer
     * 
     * @param framebuffer_ associated framebuffer. Will be used in all render passes
     * @param attachments Framebuffer attachments, will be converted to usages of each descriptor, and their end states
     */
    FlowFramebuffer(VkFramebuffer framebuffer_, const FlowRenderPassAttachments& attachments);

    FlowFramebuffer& setFramebuffer(VkFramebuffer fm);
};




/**
 * @brief Base class for all render pass sections, contains a render pass and its' settings
 * 
 */
class FlowRenderPass : public FlowSectionListRenderPass{
public:
    VkRenderPass render_pass;
protected:
    //contains clear colors and framebuffer size 
    RenderPassSettings m_settings;

    //holds one standalone image - one not associated with a flow context
    class StandaloneImageAttachment{
    public:
        VkFormat format;
        VkImageLayout final_layout;
        VkAttachmentLoadOp load_op;
        VkAttachmentStoreOp store_op;
        ClearValue clear_value;

        StandaloneImageAttachment(VkFormat format_, VkImageLayout final_layout_, VkAttachmentLoadOp load_op_, VkAttachmentStoreOp store_op_, ClearValue clear_value_);
    };
public:
    /**
     * @brief Create a FlowRenderPass with given attachments
     *  
     * @param ctx associated flow context
     * @param size framebuffer size
     * @param attachments attachments used by this render pass
     */
    FlowRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments);

    /**
     * @brief Construct a new Flow Render Pass object
     * 
     * @param ctx associated flow context
     * @param size framebuffer size
     * @param attachments attachments used by this render pass
     * @param standalone_images attachments used outside of flow context
     */
    FlowRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments, const vector<StandaloneImageAttachment>& standalone_images);
protected:
    /**
     * @brief Given a FlowFramebuffer with used images, their usages and end states, execute a render pass.
     * 
     * @param buffer command buffer to record this operation on
     * @param framebuffer the framebuffer to render into, image usages and end states
     */
    void executeRenderPass(CommandBuffer& buffer, const FlowFramebuffer& framebuffer);

    //create a framebuffer using given attachments
    VkFramebuffer createFramebuffer(const glm::uvec2& size, const FlowRenderPassAttachments& attachments);
};


/**
 * @brief Holds one FlowFrambuffer, in addition to normal render pass
 * 
 */
class FlowFramebufferRenderPass : public FlowRenderPass{
    //this framebuffer will be used during each execution
    FlowFramebuffer m_framebuffer;
public:
    /**
     * @brief Construct a new Flow Framebuffer Render Pass object
     * 
     * @param ctx associated flow context
     * @param size framebuffer size
     * @param attachments attachments used to create framebuffer and render pass
     */
    FlowFramebufferRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments);

    //execute a render pass and all subsections with the framebuffer
    virtual void execute(CommandBuffer& command_buffer);
};


/**
 * @brief Holds two framebuffers swappable at will, and a single render pass
 * 
 */
class FlowDoubleFramebufferRenderPass : public FlowRenderPass{
    FlowFramebuffer m_framebuffer_1;
    FlowFramebuffer m_framebuffer_2;
    //true if m_framebuffer_1 is bound
    bool m_first_framebuffer_bound;
public:
    /**
     * @brief Create a new double framebuffer render pass
     * 
     * @param ctx associated flow context
     * @param size framebuffer size
     * @param attachments_1 set of attachments for the first framebuffer
     * @param attachments_2 set of attachments for the second framebuffer. Must be compatible - same attachments = same formats, sizes, ...
     */
    FlowDoubleFramebufferRenderPass(FlowContext& ctx, const glm::uvec2& size, const FlowRenderPassAttachments& attachments_1, const FlowRenderPassAttachments& attachments_2);
    
    //select a framebuffer to be used during the next execute
    void swapFramebuffers(bool use_first);
    
    //select a framebuffer based on internal flag, then execute a render pass using it
    virtual void execute(CommandBuffer& buffer);
};


/**
 * @brief Holds a render pass to be used with swapchain images.
 * 
 */
class FlowSwapchainRenderPass : public FlowRenderPass{
    Swapchain& m_swapchain;
    //image currently being drawn into
    SwapchainImage m_current_image;
    //holds settings, framebuffer is set to match the current swapchain image every frame
    FlowFramebuffer m_framebuffer_settings;
public:
    /**
     * @brief Create a new swapchain render pass
     * 
     * @param ctx associated flow context
     * @param swapchain swapchain to take images from
     * @param additional_attachments additional attachments to use in addition to swapchain color image. Currently, color images are not supported - should only be one depth image or nothing
     */
    FlowSwapchainRenderPass(FlowContext& ctx, Swapchain& swapchain, const FlowRenderPassAttachments& additional_attachments);

    //get a new swapchain image, use a barrier to prepare it for being used as an attachment, then execute the render pass with its' framebuffer
    virtual void execute(CommandBuffer& command_buffer);

    //Wait until swapchain image can be drawn into
    void waitForSwapchainImage();

    //present the rendered image using the given queue
    void presentResult(Queue& queue);
    
    //acquire the next image, and save it in m_current_image. Is called automatically during execute, can be called by itself during debugging, when we don't want to run the render pass.
    void acquireImage();
};

#endif