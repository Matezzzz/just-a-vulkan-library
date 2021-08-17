#ifndef FLOW_SECTIONS_H
#define FLOW_SECTIONS_H

#include "flow_sections_base.h"
#include "../03_commands/command_buffer.h"
#include "../08_pipeline/pipeline.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;

/**
 * FlowSection
 *  - Base class for all flow sections, holds a list of descriptors used by this section, has a method for transitioning all descriptors to correct states.
 */
class FlowSection{
    //all descriptors used by this section
    vector<FlowSectionDescriptorUsage> m_descriptors_used;
public:
    /**
     * Construct flow section with given descriptor usages
     * @param usages the descriptors used by this section
     */
    FlowSection(const vector<FlowSectionDescriptorUsage>& usages);

    /**
     * Complete this section, has to be called for after creating all sections and creating descriptor pool but before executing them.
     */
    virtual void complete();
    
    /**
     * Transition all descriptors into the correct states to be used by this section. This is done by inserting multiple memory barriers.
     * @param buffer the buffer to record descriptor transitions into
     * @param flow_context descriptor context for this section
     */
    void transition(CommandBuffer& buffer, FlowDescriptorContext& flow_context);

    /**
     * Execute this section on the given command buffer. Section must be completed first, can be called multiple times.
     * @param command_buffer the buffer to record this section to
     */
    virtual void execute(CommandBuffer& command_buffer) = 0;
    
    /**
     * Run calls transition() and then execute().
     */
    void run(CommandBuffer& command_buffer, FlowDescriptorContext& flow_context);
};


/**
 * FlowTransitionSection
 *  - Used to transition one or multiple descriptors to target state
 */
class FlowTransitionSection : public FlowSection{
public:
    /**
     * Constructor for transitioning one descriptor to target state
     * @param descriptor the descriptor to transition
     */
    FlowTransitionSection(const FlowSectionDescriptorUsage& descriptor);

    /**
     * Constructor for transitioning multiple descriptors to target states
     * @param descriptors vector of descriptors to transition
     */
    FlowTransitionSection(const vector<FlowSectionDescriptorUsage>& descriptors);
    
    /**
     * Execute override - does nothing. All work is already done automatically during transition() in parent FlowSection()
     */
    virtual void execute(CommandBuffer&);
};




//Was used when command buffers were recorded only once before starting the main loop, hence the need to transition all descriptors to a same layout as if the loop would have just ended
//Now unused, but might be useful in the future
/*class FlowIntoLoopTransitionSection : public FlowTransitionSection{
public:
    template<typename... Args>
    FlowIntoLoopTransitionSection(int img_count, const Args&... args) : FlowTransitionSection(getImageUsages(img_count, args...))
    {}
private:
    template<typename... Args>
    vector<FlowSectionDescriptorUsage> getImageUsages(int img_count, const Args&... args){
        vector<PipelineImageState> all_image_states(img_count);
        fillImageUsages(all_image_states, args...);
        vector<FlowSectionDescriptorUsage> image_usages;
        for (int i = 0; i < img_count; i++){
            if (all_image_states[i].layout != VK_IMAGE_LAYOUT_UNDEFINED){
                image_usages.push_back(FlowSectionDescriptorUsage{i, DescriptorUsageStage{all_image_states[i].last_use}, all_image_states[i]});
            }
        }
        return image_usages;
    }
    void fillImageUsages(vector<PipelineImageState>&){}
    template<typename... Args>
    void fillImageUsages(vector<PipelineImageState>& usages, const SectionList& l, const Args&... args){
        l.getLastImageStates(usages);
        fillImageUsages(usages, args...);
    }
};*/




/**
 * FlowClearColorSection
 *  - Fills one image with given color
 */
class FlowClearColorSection : public FlowSection{
    //the color the image will be filled with
    VkClearValue m_clear_value;
    //the image to fill
    const ExtImage& m_image;
public:
    /**
     * Construct FlowClearColorSection given descriptor context, index and value to clear with
     * @param ctx descriptor context
     * @param descriptor_index index of image to clear
     * @param clear_value the value to clear with
     */
    FlowClearColorSection(FlowDescriptorContext& ctx, int descriptor_index, VkClearValue clear_value);

    /**
     * Execute calls one cmdClearColor on target image to fill it with given color.
     * @param buffer the command buffer to record the clear operation on
     */
    virtual void execute(CommandBuffer& buffer);
};



/**
 * FlowPipelineSectionDescriptors
 *  - Holds descriptor context, data about how descriptors are used during a section and how to update them
 */
class FlowPipelineSectionDescriptors{
    //descriptor context associated with latter descriptors
    FlowDescriptorContext& m_context;
    //list of descriptors and their update infos
    vector<FlowPipelineSectionDescriptorUsage> m_descriptors;
public:
    /**
     * Construct a list of descriptor usages and update infos given descriptor context and usages
     * @param ctx descriptor context
     * @param descriptors list of descriptors used
     */
    FlowPipelineSectionDescriptors(FlowDescriptorContext& ctx, const vector<FlowPipelineSectionDescriptorUsage>& descriptors);

    /**
     * Get a vector of FlowSectionDescriptorUsage - go through all descriptors and get usage for each one, then add them to a vector and return
     */
    vector<FlowSectionDescriptorUsage> getDescriptorUsages() const;

    /**
     * Get a vector od DescriptorUpdateInfos - go through all descriptos and get update info for each one, then add them to a vector and return
     */
    vector<DescriptorUpdateInfo> getUpdateInfos() const;
};



/**
 * FlowPipelineSection
 *  - Base class for flow sections involving use of a pipeline, holds pipeline context and pipeline itself
 */
class FlowPipelineSection : public FlowSection{
protected:
    //reference to context of current pipeline
    PipelineContext& m_context;
    //Pipeline to use
    Pipeline m_pipeline;
public:
    /**
     * Construct FlowPipelineSection for a compute pipeline
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section
     */
    FlowPipelineSection(DirectoryPipelinesContext& ctx, const string& name, const vector<FlowSectionDescriptorUsage>& usages);
    
    /**
     * Construct FlowPipelineSection for a graphics pipeline
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowPipelineSection(DirectoryPipelinesContext& ctx, const string& name, const vector<FlowSectionDescriptorUsage>& usages, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);
    
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
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section and their update infos
     */
    FlowSimplePipelineSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages);
    
    /**
     * Construct FlowSimplePipelineSection for a graphics pipeline with given descriptor usages
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section and their update infos
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowSimplePipelineSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
            const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    /**
     * Complete allocates and updates all descriptors in associated descriptor set
     */
    virtual void complete();

    /**
     * Bind pipeline with current descriptor set
     */
    void bind(CommandBuffer& buffer);
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
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors to use and their update infos
     * @param compute_size global dispatch size (how many local work groups to dispatch)
     */
    FlowComputeSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages, Size3 compute_size);

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
     * @param ctx list of pipeline context to pick the one with right name from
     * @param name name of shader directory to use
     * @param usages descriptors used during this section and their update infos
     * @param vertex_count how many vertices to render
     * @param pipeline_info all data required for graphics pipeline creation
     * @param render_pass the render pass, during which the pipeline will be used
     * @param subpass_index index of subpass in given render pass during which pipeline will be used, defaults to 0 (first subpass)
     */
    FlowGraphicsSection(DirectoryPipelinesContext& ctx, const string& name, const FlowPipelineSectionDescriptors& usages,
            uint32_t vertex_count, const PipelineInfo& pipeline_info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    /**
     * Bind graphics pipeline and execute it, rendering given number of vertices
     */
    virtual void execute(CommandBuffer& buffer);
};


/**
 * FlowPushConstantSection
 *  - This class is used to extend any pipeline section to support push constants
 */
template<typename T>
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
    FlowPushConstantSection(DirectoryPipelinesContext& ctx, const string& name, const Args&... args) :
        T(ctx, name, args...), m_push_constant_data(this->m_context.createPushConstantData())
    {}

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



/**
 * FlowLoopPushConstantSection
 *  - Can be used to extend FlowPushConstantSection<T> to run many times. Writes is_even_iteration push constant every iteration
 */
template<typename T>
class FlowLoopPushConstantSection : public T{
    //total amount of iterations
    uint32_t m_iteration_count;
    //reference to descriptor context
    FlowDescriptorContext& m_descriptor_context;
public:
    /**
     * Construct FlowLoopPushConstantSection with given iteration count, given descriptor context, shader directory context and additional parameters required by T
     * @param iteration_count how many iterations should the section T be repeated
     * @param ctx descriptor context
     * @param shader_ctx shader directory context
     * @param args arguments required by T (all after shader context)
     */
    template<typename... Args>
    FlowLoopPushConstantSection(uint32_t iteration_count, FlowDescriptorContext& ctx, DirectoryPipelinesContext& shader_ctx, const Args&... args) : T(shader_ctx, args...), m_iteration_count(iteration_count), m_descriptor_context(ctx)
    {}
    /**
     * Execute loop section - record section T multiple times
     * @param buffer the command buffer to record to
     */
    virtual void execute(CommandBuffer& buffer){
        //true during the first pass through the loop, false the rest of the time
        bool first = true;
        //go through all iterations
        for (uint32_t i = 0; i < m_iteration_count; i++){
            //no transition is needed the first time - descriptors were already transitioned when this section started
            if (first){
                first = false;
            }else{
                T::transition(buffer, m_descriptor_context);
            }
            //write is_even_iteration push constant
            T::getPushConstantData().write("is_even_iteration", (i % 2 == 0) ? 1U : 0U);
            //execute T
            T::execute(buffer);
        }
    }
};




/**
 * FlowSectionList
 *  - Holds list of sections that can be completed and executed as one. Sections are given as pointers created with new, class is responsible for managing them.
 */
class FlowSectionList : public FlowSection{
    //descriptor context associated with this list
    FlowDescriptorContext& m_context;
    //pointers to all sections that are subsets of this one
    vector<unique_ptr<FlowSection>> m_sections;
public:
    /**
     * Initialize FlowSectionList with descriptor context and pointers to all subsections
     * @param ctx flow descriptor context
     * @param args any number of pointers to sections (base class must be FlowSection)
     */
    template<typename... Args>
    FlowSectionList(FlowDescriptorContext& ctx, Args*... args) : FlowSection({}), m_context(ctx){
        //reserve space for all subsections
        m_sections.reserve(sizeof...(args));
        //call addSection function to add all given sections to m_sections vector
        addSections(args...);
    }

    /**
     * Call complete() on all subsections.
     */
    virtual void complete();

    /**
     * Run all subsections (call transition & execute on each one)
     * @param command_buffer the buffer to record subsections into
     */
    virtual void execute(CommandBuffer& command_buffer);

    //Currently unused, might be helpful in the future
    /*void getLastImageStates(vector<PipelineImageState>& states) const{
        for (const unique_ptr<FlowSection>& section : *this){
            for (const FlowSectionDescriptorUsage& img : section->m_descriptors_used){
                states[img.descriptor_index] = img.toImageState(false);
            }
        }
    }*/
private:
    //is called when there are no more sections left to add, does nothing
    void addSections();
    //Is called while there are sections still to add
    template<typename T, typename... Args>
    void addSections(T* ptr, Args*... args){
        //make section into a unique_ptr so I don't have to deal with pointer management, then add it to the list
        m_sections.emplace_back(unique_ptr<T>(ptr));
        //call addSections on remaining sections
        addSections(args...);
    }
};


#endif