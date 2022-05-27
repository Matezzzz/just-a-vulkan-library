#ifndef FLOW_SECTIONS_H
#define FLOW_SECTIONS_H

#include "../03_commands/command_buffer.h"
#include "../03_commands/command_pool.h"
#include "../04_memory_objects/local_object_creator.h"
#include "../08_pipeline/pipeline.h"
#include "../09_utilities/vector_ext.h"
#include "../10_flow/flow_base.h"

#include <memory>

using std::unique_ptr;
using std::make_unique;



/**
 * FlowSection
 *  - Base class for all flow sections, holds a list of descriptors used by this section, has a method for transitioning all descriptors to correct states.
 */
class FlowSection{
protected:
    FlowContext& m_context;
public:
    /**
     * Construct flow section with given descriptor usages
     * @param usages the descriptors used by this section
     */
    FlowSection(FlowContext& ctx);

    /**
     * Complete this section, has to be called for after creating all sections and creating descriptor pool but before executing them.
     */
    virtual void complete();
    
    /**
     * Transition all descriptors into the correct states to be used by this section. This is done by inserting multiple memory barriers.
     * @param buffer the buffer to record descriptor transitions into
     * @param flow_context descriptor context for this section
     */
    virtual void transition(CommandBuffer& buffer);

    /**
     * Execute this section on the given command buffer. Section must be completed first, can be called multiple times.
     * @param command_buffer the buffer to record this section to
     */
    virtual void execute(CommandBuffer& command_buffer) = 0;
    
    /**
     * Run calls transition() and then execute().
     */
    void run(CommandBuffer& command_buffer);

    virtual ~FlowSection();
protected:
    void transitionDescriptors(CommandBuffer& buffer, const vector<FlowSectionDescriptorUsage>& descriptors_used);
};



/**
 * FlowTransitionSection
 *  - Used to transition one or multiple descriptors to target state
 */
class FlowTransitionSection : public FlowSection{
protected:
    //all descriptors used by this section
    vector<FlowSectionDescriptorUsage> m_descriptors_used;
public:
    /**
     * Constructor for transitioning one descriptor to target state
     * @param descriptor the descriptor to transition
     */
    FlowTransitionSection(FlowContext& ctx, const FlowSectionDescriptorUsage& descriptor);

    /**
     * Constructor for transitioning multiple descriptors to target states
     * @param descriptors vector of descriptors to transition
     */
    FlowTransitionSection(FlowContext& ctx, const vector<FlowSectionDescriptorUsage>& descriptors);
    
    virtual void transition(CommandBuffer& buffer);

    /**
     * Execute override - does nothing. All work is already done automatically during transition() in parent FlowSection()
     */
    virtual void execute(CommandBuffer&);

    void updateDescriptor(int index, int new_descriptor_index);
};




/**
 * FlowClearColorSection
 *  - Fills one image with given color
 */
class FlowClearColorSection : public FlowTransitionSection{
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
    FlowClearColorSection(FlowContext& ctx, int descriptor_index, VkClearValue clear_value);

    /**
     * Execute calls one cmdClearColor on target image to fill it with given color.
     * @param buffer the command buffer to record the clear operation on
     */
    virtual void execute(CommandBuffer& buffer);
};







/**
 * FlowSectionList
 *  - Holds list of sections that can be completed and executed as one. Sections are given as pointers created with new, class is responsible for managing them.
 */
class FlowSectionListBase : private vector<unique_ptr<FlowSection>> {
public:
    //create without any subsections
    FlowSectionListBase();

    /**
     * Initialize FlowSectionList with descriptor context and pointers to all subsections
     * @param ctx flow descriptor context
     * @param args any number of pointers to sections (base class must be FlowSection)
     */
    FlowSectionListBase(const vector<FlowSection*>& sections);
protected:
    //call complete on all subsections
    void completeAll();
    
    //call transition on all subsections
    void transitionAll(CommandBuffer& buffer);

    //call execute on all subsections
    void executeAll(CommandBuffer& buffer);

    //call run on all subsections
    void runAll(CommandBuffer& buffer);
private:
    //is called when there are no more sections left to add, does nothing
    void addSections();
public:
    //Is called while there are sections still to add
    template<typename T, typename... Args>
    void addSections(T* ptr, Args*... args){
        //make section into a unique_ptr so I don't have to deal with pointer management, then add it to the list
        emplace_back(unique_ptr<T>(ptr));
        //call addSections on remaining sections
        addSections(args...);
    }
    void addSections(const vector<FlowSection*>& sections);
};




class FlowSectionList : public FlowSection, public FlowSectionListBase{
public:
    /**
     * @brief Create with given sections and context
     * 
     * @param ctx associated flow context
     * @param sections sections to add
     */
    FlowSectionList(FlowContext& ctx, const vector<FlowSection*>& sections);

    //run complete on all sections
    virtual void complete();

    //call run on all sections
    virtual void execute(CommandBuffer& command_buffer);
};


#endif