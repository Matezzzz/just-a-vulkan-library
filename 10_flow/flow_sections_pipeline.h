#ifndef FLOW_SECTIONS_PIPELINE_H
#define FLOW_SECTIONS_PIPELINE_H

#include "../10_flow/flow_sections.h"


/**
 * FlowPipelineSectionDescriptors
 *  - Holds descriptor context, data about how descriptors are used during a section and how to update them
 */
class FlowPipelineSectionDescriptors{
    //list of descriptors and their update infos
    vector<FlowPipelineSectionDescriptorUsage> m_descriptors;
public:
    /**
     * Construct a list of descriptor usages and update infos given descriptor context and usages
     * @param ctx descriptor context
     * @param descriptors list of descriptors used
     */
    FlowPipelineSectionDescriptors(const vector<FlowPipelineSectionDescriptorUsage>& descriptors);

    /**
     * @brief Add a descriptor usage to the list
     * 
     * @param usage the usage to add
     */
    FlowPipelineSectionDescriptors& add(const FlowPipelineSectionDescriptorUsage& usage);
    
    /**
     * @brief Add a descriptor usage to the list
     * 
     * @param usage the usage to add
     */
    FlowPipelineSectionDescriptors add(const FlowPipelineSectionDescriptorUsage& usage) const;

    /**
     * Get a vector of FlowSectionDescriptorUsage - go through all descriptors and get usage for each one, then add them to a vector and return
     */
    vector_ext<FlowSectionDescriptorUsage> getDescriptorUsages() const;

    /**
     * Get a vector od DescriptorUpdateInfos - go through all descriptos and get update info for each one, then add them to a vector and return
     */
    vector<DescriptorUpdateInfo> getUpdateInfos(FlowContext& flow_ctx) const;
};



/**
 * FlowPipelineSection
 *  - Base class for flow sections involving use of a pipeline, holds pipeline context and pipeline itself
 */
class FlowPipelineSection : public FlowTransitionSection{
protected:
    //reference to context of current pipeline
    PipelineContext& m_pipeline_context;
    //Pipeline to use
    Pipeline m_pipeline;
public:
    /**
     * Construct FlowPipelineSection for a compute pipeline
     * @param flow_ctx associated flow context
     * @param ctx pipeline context to use
     * @param usages descriptors used during this section
     * @param pipeline the pipeline to use
     */
    FlowPipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const vector<FlowSectionDescriptorUsage>& usages, Pipeline&& pipeline);
        
    /**
     * Get a reference to shader context (return m_context)
     */
    PipelineContext& getShaderContext();
};



/**
 * FlowSimplePipelineSection
 *  - Specializes FlowPipelineSection for pipelines that use only one descriptor set
 */
class FlowSimplePipelineSection : public FlowPipelineSection{
    //the descriptor set used by this pipeline
    DescriptorSet m_descriptor_set;
    //Holds update infos for all descriptors in the descriptor set above
    vector<DescriptorUpdateInfo> m_descriptor_update_infos;
public:
    /**
     * Construct FlowSimplePipelineSection for a compute pipeline with given descriptor usages
     * @param flow_ctx associated flow context
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section and their update infos
     */
    FlowSimplePipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const FlowPipelineSectionDescriptors& usages, Pipeline&& pipeline);
    
    /**
     * Construct FlowSimplePipelineSection for a graphics pipeline with given descriptor usages
     * @param flow_ctx associated flow context
     * @param ctx list of pipeline context to pick the one with right name from
     * @param usages descriptors used during this section and their update infos
     * @param usages_base descriptors used during this section, not allocated as descriptor sets (e.g. vertex buffers)
     * @param pipeline pipeline used
     */
    FlowSimplePipelineSection(FlowContext& flow_ctx, PipelineContext& ctx, const FlowPipelineSectionDescriptors& usages,
        const vector_ext<FlowSectionDescriptorUsage>& usages_base, Pipeline&& pipeline);

    /**
     * Complete allocates and updates all descriptors in associated descriptor set
     */
    virtual void complete();

    /**
     * Bind pipeline with current descriptor set
     */
    void bind(CommandBuffer& buffer);

    /**
     * Update a descriptor using given info
     * @param info info about the descriptor to update
     */
    void updateDescriptor(const DescriptorUpdateInfo& info, int new_descriptor_index);
};


/**
 * FlowComputeSection
 *  - Represents one pass of a compute shader, with number of local work groups running in each dimension
 */
class FlowComputeSection : public FlowSimplePipelineSection{
    //number of local work groups to run in each dimension
    Size3 m_compute_size;
public:
    /**
     * Create a section with a compute shader from given context with given name.
     * @param flow_ctx associated flow context
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors to use and their update infos
     * @param compute_size global dispatch size (how many local work groups to dispatch)
     */
    FlowComputeSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size);

    /**
     * Execute compute shader on given command buffer.
     * @param buffer command buffer to record shader execution on
     */
    virtual void execute(CommandBuffer& buffer);
};






/**
 * FlowComputeSection
 *  - Represents one graphics pass of a shader directory, with given vertex count
 */
class FlowGraphicsSection : public FlowSimplePipelineSection{
    //number of vertices to render
    uint32_t m_vertex_count;
public:
    /**
     * Create a section with all graphics shaders in given directory from given context 
     * @param flow_ctx associated flow context
     * @param ctx the pipeline context to use
     * @param name the name of the shader directory
     * @param usages descriptors used during this section and their update infos
     * @param vertex_count how many vertices to render
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowGraphicsSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
            uint32_t vertex_count, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    /**
     * Create a section with all graphics shaders in given directory from given context 
     * @param flow_ctx associated flow context
     * @param ctx the pipeline context to use
     * @param name the name of the shader directory
     * @param usages descriptors used during this section and their update infos
     * @param usages_base descriptors used but without allocated sets (e.g. vertex buffers)
     * @param vertex_count how many vertices to render
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowGraphicsSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
            const vector_ext<FlowSectionDescriptorUsage>& usages_base, uint32_t vertex_count,
            const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    /**
     * Bind graphics pipeline and execute it, rendering given number of vertices
     */
    virtual void execute(CommandBuffer& buffer);
};





/**
 * @brief A graphics section with an associated vertex buffer
 * 
 */
class FlowGraphicsVertexSection : public FlowGraphicsSection{
    int m_vertex_buffer_index;
public:
    /**
     * @brief Create new FlowGraphicsVertexSection
     * 
     * @param flow_ctx associated flow context
     * @param ctx the pipeline context to use
     * @param name name of shader directory to use
     * @param usages descriptors used during this section and their update infos
     * @param vertex_buffer_descriptor_index the vertex buffer to use. Vertex count is taken from FlowContext.
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowGraphicsVertexSection(FlowContext& flow_ctx, DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
        int vertex_buffer_descriptor_index, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    //bind the vertex buffer, then execute the pipeline
    virtual void execute(CommandBuffer& buffer);
};


// concept
template<class T>
concept DerivedFromFlowSection = std::is_base_of<FlowSection, T>::value;

/**
 * FlowPushConstantSection
 *  - This class is used to extend any pipeline section to support push constants
 */
template<DerivedFromFlowSection T>
class FlowPushConstantSection : public T{
    //push constant data for given pipeline section
    PushConstantData m_push_constant_data;
public:
    /**
     * Construct FlowPushConstantSection given context, name, and derived-class specific arguments
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param args (for simple compute section) const FlowPipelineSectionDescriptors& usages, Size3 compute_size
     * @param args (for simple graphics section) const FlowPipelineSectionDescriptors& usages, uint32_t vertex_count, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0
     */
    template<typename... Args>
    FlowPushConstantSection(FlowContext& flow_context, DirectoryPipelinesContext& ctx, const string& name, const Args&... args) :
        T(flow_context, ctx, name, args...), m_push_constant_data(this->m_pipeline_context.createPushConstantData())
    {
        flow_context.registerPushConstants(name, m_push_constant_data);
    }

    /**
     * Return a reference to push constant data of this section
     */
    PushConstantData& getPushConstantData(){
        return m_push_constant_data;
    }

    /**
     * Execute this section - push all constants, then execute parent class
     * @param buffer the buffer to record execution on
     */
    virtual void execute(CommandBuffer& buffer){
        buffer.cmdPushConstants(this->m_pipeline, m_push_constant_data);
        T::execute(buffer);
    }
};


//typedefs for most common uses of FlowPushConstantSection<>
typedef FlowPushConstantSection<FlowComputeSection> FlowComputePushConstantSection;
typedef FlowPushConstantSection<FlowGraphicsSection> FlowGraphicsPushConstantSection;
typedef FlowPushConstantSection<FlowGraphicsVertexSection> FlowGraphicsVertexPushConstantSection;


/**
 * FlowLoopPushConstantSection
 *  - Can be used to extend FlowPushConstantSection<T> to run many times. Writes is_even_iteration push constant every iteration
 */
class FlowComputeLoopPushConstantSection : public FlowComputePushConstantSection{
    //total amount of iterations
    uint32_t m_iteration_count;
public:
    /**
     * Construct FlowLoopPushConstantSection with given iteration count, given descriptor context, shader directory context and additional parameters required by T
     * @param ctx descriptor context
     * @param shader_ctx shader directory context
     * @param usages which descriptors are used during this section
     * @param compute_size how large should the compute space be
     * @param iteration_count how many iterations should the section T be repeated
     */
    FlowComputeLoopPushConstantSection(FlowContext& ctx, DirectoryPipelinesContext& shader_ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size, uint32_t iteration_count);
    /**
     * Execute loop section - record section T multiple times
     * @param buffer the command buffer to record to
     */
    virtual void execute(CommandBuffer& buffer);
};


#endif