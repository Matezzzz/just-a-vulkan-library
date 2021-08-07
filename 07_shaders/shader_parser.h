#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include "../04_memory_objects/mixed_buffer.h"
#include "read_shader_directory.h"
#include "descriptor_data.h"
#include "parsing_utils.h"


/**
 * PushConstantLayout
 *  - Holds layout of push constants for one single stage when initialized, can be merged with layouts of other stages to hold layout for all shaders
 */
class PushConstantLayout : public MixedBufferLayout{
    //Holds stage flags for each variable in mixed buffer layout
    vector<VkShaderStageFlags> m_stage_flags;
public:
    //add push constants from given data for given stage
    void addPushConstants(const PushConstantShaderData& data, VkShaderStageFlags stage);
    
    /**
     * Combine push constant layout with another one - matching variables have their stage flags combined, overlapping variables generate errors.
     * @param p other push constant layout to combine with
     * @param stage - other layout stage flags
     */
    void combineWith(const PushConstantLayout& p, VkShaderStageFlags stage);

    //get a vector of push constant ranges - this is called when creating pipeline layout
    vector<VkPushConstantRange> getPushConstantRanges() const;

    //return combined flags of all stages that will be using these push constants
    VkShaderStageFlags getAllStages() const;

    //add new variable to layout
    void push_back(const BufferType& t, VkShaderStageFlags flags);

    //insert variable at the given index - is used to keep variables sorted by offset
    void insert(const BufferType& t, VkShaderStageFlags flags, uint32_t i);
private:
    //recalculate size after adding new variables
    void recalculateSize();
};



/**
 * ShaderDataDescriptorSet
 *  - Holds all descriptors belonging to one set
 */
class ShaderDataDescriptorSet : public vector<DescriptorData>
{
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    ShaderDataDescriptorSet* m_shared_set = nullptr;
public:
    //add given descriptor to the set
    void addDescriptor(const DescriptorData& descriptor_data);

    //combine with another descriptor set - combine both sets and print errors for invalid overlaps
    void combineWith(const ShaderDataDescriptorSet& d);

    //Create layout for the set, or return it, if it already exists
    VkDescriptorSetLayout getOrCreateLayout();

    //make shared descriptor set - All descriptors have to be set only once for both sets, and it's allocated only once
    //! not tested yet, might not work
    void makeShared(ShaderDataDescriptorSet& d);

    //whether this set is shared
    bool isShared() const;

    //find index of descriptor with given name
    uint32_t find(const string& descriptor_name) const;

    //create uniform buffer data for descriptor with given binding
    UniformBufferLayoutData createUniformBufferData(uint32_t binding);

    //create uniform buffer data for descriptor of given name
    UniformBufferLayoutData createUniformBufferData(const string& name);
private:
    //return a reference to descriptor of given binding
    inline DescriptorData& getDescriptor(uint32_t binding_i);
    //return a reference to descriptor of given binding
    inline const DescriptorData& getDescriptor(uint32_t binding_i) const;
};



/**
 * ShaderDataDescriptorSetVector
 *  - Holds all descriptor sets in ShaderDataInfo
 */
class ShaderDataDescriptorSetVector : public vector<ShaderDataDescriptorSet>{
public:
    //add descriptor to the sets
    void addDescriptor(const DescriptorData& descriptor_data);

    //combine all subsets that overlap, add all that don't
    void combineWith(const ShaderDataDescriptorSetVector& d);
    
    //Create layout of set with the given index, or return it, if it already exists
    VkDescriptorSetLayout getOrCreateLayout(uint32_t set_index);

    //make set of given index shared - All descriptors have to be set only once for both sets, and it's allocated only once
    //! not tested yet, might not work
    void makeShared(uint32_t set_index, ShaderDataDescriptorSet& set);
private:
    //return a reference to set of the given index
    inline ShaderDataDescriptorSet& getSet(uint32_t set_i);
};



/**
 * ShaderDataInfo
 *  - At initialization, holds information about one shader stage - all descriptor sets, push constants, inputs and outputs
 *  - Can be combined with other shader stages to form ShaderDataInfo for the all shader stages
 */
class ShaderDataInfo
{
private:
    VkShaderStageFlags       m_stage;
    ShaderDataDescriptorSetVector m_descriptor_sets;
    PushConstantLayout       m_push_constants;
    vector<ShaderInOutData>  m_inputs;
    vector<ShaderInOutData>  m_outputs;
public:
    //create empty ShaderDataInfo with empty vectors and stage
    ShaderDataInfo();

    //read descriptor sets, push constants, inputs and outputs from GLSL shader code
    ShaderDataInfo(const string& glsl_shader_code, VkShaderStageFlags shader_stage);

    //combine with other shader data - add inputs and outputs, join together push constants and descriptor sets while checking for incompatible overlaps
    void combineWithShaderData(const ShaderDataInfo& info);

    //make shared descriptor set - All descriptors have to be set only once for both sets, and it's allocated only once
    //! not tested yet, might not work
    void makeShared(uint32_t set_i, ShaderDataDescriptorSet& set);

    VkShaderStageFlags getStage() const;
    ShaderDataDescriptorSetVector& getSets();
    const PushConstantLayout& getPushConstantLayout() const;
private:
    //read properties from one shader line
    void parseShaderLine  (parse::string_view line);
    
    /**
     * Read descriptor and add it to class
     * @param name_type e.g. 'texture2D wood'
     * @param parameters set, binding, qualifiers, ...
     * @param is_storage_buffer the distinction before storage buffer and uniform buffer cannot be drawn based on other parameters to this function, so it's passed as a bool
     */
    void readDescriptor   (parse::string_view name_type, const DescriptorParameterVector& parameters, bool is_storage_buffer = false);

    //Read input and add it to the internal array
    void readShaderIn     (parse::string_view name_type, const DescriptorParameterVector& parameters);
    //Read output and add it to the internal array
    void readShaderOut    (parse::string_view name_type, const DescriptorParameterVector& parameters);
};


#endif