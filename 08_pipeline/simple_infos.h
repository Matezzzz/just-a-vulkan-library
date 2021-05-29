#ifndef SIMPLE_INFOS_H
#define SIMPLE_INFOS_H

#include "../00_base/vulkan_base.h"


class VertexInputInfo
{
    VkPipelineVertexInputStateCreateInfo m_info;
    vector<VkVertexInputBindingDescription> m_buffer_descriptions;
    vector<VkVertexInputAttributeDescription> m_data_descriptions; 
public:
    /**
     * Creates new vertex input info with no buffers and no data.
     */
    VertexInputInfo();

    /**
     * Adds data, configured as floats, to the info. Locations are incremented for every added parameter, bindings for every added buffer.
     * @param data_setup vector describing data counts in floats, example - for position and texture coordinates it would be {3, 2}.
     * @param input_rate describes whether next data is went to each vertex or on each instance. Possible values - VK_VERTEX_RATE_INPUT_RATE_*** - VERTEX, INSTANCE
     */
    VertexInputInfo& addFloatBuffer(vector<uint32_t> data_setup, VkVertexInputRate input_rate = VK_VERTEX_INPUT_RATE_VERTEX);

    /**
     * Return a pointer to info object.
     */
    const VkPipelineVertexInputStateCreateInfo* getInfo() const;
};



class AssemblyInfo
{
    VkPipelineInputAssemblyStateCreateInfo m_info;
public:
    /**
     * Creates a new assembly create info for a pipeline.
     * Default parameters are: TRIANGLE_LIST topology, no restart on special primitive
     */
    AssemblyInfo();

    /**
     * Sets topology(draw type) of primitive
     * @param topology draw type. Possible values - VK_PRIMITIVE_TOPOLOGY_***, most common TRIANGLE_LIST, LINE_STRIP, POINT_LIST  
     */
    AssemblyInfo& setTopology(VkPrimitiveTopology topology);
    
    /**
     * In indexed draws, a special index value, 0xFFFF or 0xFFFFFFFF, will restart the assembly of primitives
    */
    AssemblyInfo& enablePrimitiveRestart();

    /**
     * There is no index value to restart the assembly of primitives
     */
    AssemblyInfo& disablePrimitiveRestart();

    /**
     * Return a pointer to info object.
     */
    const VkPipelineInputAssemblyStateCreateInfo* getInfo() const;
};



class TesselationInfo
{
    VkPipelineTessellationStateCreateInfo m_info;
public:
    /**
     * Create new tesselation info with 0 vertices per patch. If kept this way, getInfo() will return a null pointer, creating a pipeline without tesselation
     */
    TesselationInfo();

    /**
     * Set new tesselated point count.
     * @param count count of vertices per patch. More than 32 don't have to be supported.
     */
    TesselationInfo& setPointCount(uint32_t count);

    /**
     * Return a reference to info object.
     */
    const VkPipelineTessellationStateCreateInfo* getInfo() const;
};



class DynamicInfo
{
    uint32_t m_dynamic_flags;
    vector<VkDynamicState> m_dynamic_states;
    VkPipelineDynamicStateCreateInfo m_info;
public:
    /**
     * Create new dynamic info for pipeline creation
     * Default parameters - No dynamic states defined
     */
    DynamicInfo();

    /**
     * Adds new dynamic states defined by dynamic flags to info.
     * @param dynamic_state the state to add, possible values: VK_DYNAMIC_STATE_*** - VIEWPORT, SCISSOR, ...
     */
    DynamicInfo& addDynamicState(VkDynamicState dynamic_state);

    /**
     * Return a reference to info object.
     */
    const VkPipelineDynamicStateCreateInfo* getInfo() const;
};


class PipelineLayoutInfo
{
    vector<VkDescriptorSetLayout> m_descriptor_set_layouts;
    vector<VkPushConstantRange> m_push_constants;
    VkPipelineLayoutCreateInfo m_info;
public:
    PipelineLayoutInfo();
    VkPipelineLayout create() const;
    PipelineLayoutInfo& addDescriptorSetLayout(VkDescriptorSetLayout desciptor_set_layout);
    PipelineLayoutInfo& addPushConstant(const VkPushConstantRange& push_constant);
    PipelineLayoutInfo& setPushConstants(const vector<VkPushConstantRange>& push_constants);
};

#endif