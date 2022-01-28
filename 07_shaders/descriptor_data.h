#ifndef DESCRIPTOR_DATA_H
#define DESCRIPTOR_DATA_H

#include "../00_base/vulkan_base.h"
#include "../04_memory_objects/mixed_buffer.h"
#include "parsing_utils.h"


/**
 * SubsetVariable
 *  - this represents subset variable of uniform buffer or push constant
 */
class SubsetVariable{
public:
    //GLSL variable type - anything from bool to mat4x4
    ShaderVariableType type;
    string name;
    //how many variables in array
    uint32_t count;
    //offset from beginning of structure, if it is explicitly specified
    uint32_t offset;
    SubsetVariable(ShaderVariableType type_, const string& name_, uint32_t count = 1, uint32_t offset = OFFSET_NOT_SPECIFIED);

    bool operator==(const SubsetVariable& v) const;

    bool usesDefaultOffset() const;
};



/**
 * SubsetVariableVector
 *  - holds all subset variables for one uniform / push constant
 */
class SubsetVariableVector : public vector<SubsetVariable>{
public:
    //whether all variables in respective vectors are identical
    bool operator==(const SubsetVariableVector& d) const;
    //convert all variables to string
    string str() const;
};



/**
 * InputAttachmentDescriptorData
 *  - holds all additional data compared to normal descriptor for input attachment
 */
struct InputAttachmentDescriptorData{
    uint32_t input_attachment_index;
};



/**
 * Specifies shader input / output
 * Input - vertex data in vertex shader
 * Output - fragment color in frament shader
 */
struct ShaderInOutData{
    uint32_t location;
    ShaderVariableType type;
    string name;
};



/**
 * PushConstantShaderData
 *  - holds all information about a push constant - it's name and subset variables
 */
class PushConstantShaderData{
    const string m_name;
    const SubsetVariableVector m_variables;
public:
    PushConstantShaderData(const string& name, const SubsetVariableVector* variables);
    const string& getName() const;
    const SubsetVariableVector& getVariables() const;
};




#define PARAMETER_NOT_FOUND ~0U
#define PARAMETER_VALUE_NONE ~1U
/**
 * DescriptorParameter
 *  - Holds parameters specified for each descriptor - these can be set, binding, write-only, ...
 */
class DescriptorParameter{
public:
    string name;
    uint32_t value;
    
    //value is PARAMETER_VALUE_NONE if none is specified
    DescriptorParameter(const string& name, uint32_t value = PARAMETER_VALUE_NONE);
};




/**
 * DescriptorParameterVector - holds all parameters for one descriptor
 */
class DescriptorParameterVector : public vector<DescriptorParameter>{
public:
    //reads string in format: "param1=XX,param2=YY,param3" and converts it into vector of parameters
    DescriptorParameterVector(parse::string_view parameters);

    //get value of parameter with given name
    uint32_t get(const string& name) const;

    //check whether parameter of given name exists
    bool exists(const string& name) const;
private:
    //read int from given string view
    static uint32_t readInt(parse::string_view v);
};




class DescriptorSetLayoutBinding;


/**
 * DescriptorData
 *  - Data for one descriptor in shaders
 */
class DescriptorData{
    uint32_t m_set;
    uint32_t m_binding;
    //shader stages in which the descriptor is used
    VkShaderStageFlags m_stage;
    //is this an mage, sampler, input attachment, ...
    DescriptorType m_type;
    string m_name;
    //if descriptor is an array, number of elements
    uint32_t m_count;
    //Additional data for specific descriptor types - is pointer to BufferUniformDescriptorData if type is UNIFORM_BUFFER or STORAGE_BUFFER, and to InputAttachmentData in case of INPUT_ATTACHMEMT type
    void* m_additional_data = nullptr;
    //Qualifiers - whether descriptor is write-only, read-only, ...
    vector<DescriptorQualifier> m_qualifiers;
public:
    //Create invalid descriptor data
    DescriptorData();
    
    //Construct descriptor data from given info
    DescriptorData(parse::string_view name_type_info, const DescriptorParameterVector& parameters, bool is_storage_buffer, VkShaderStageFlags stage, bool is_push_constant);
    
    DescriptorData(const DescriptorData& d);
    DescriptorData& operator=(const DescriptorData& d);
    //check whether name, type and count of descriptor datas are equal
    bool operator==(const DescriptorData& d) const;
    ~DescriptorData();

    //Is used when creating descriptor set layout
    VkDescriptorSetLayoutBinding getLayoutBinding() const;
    uint32_t getSet() const;
    uint32_t getBinding() const;
    DescriptorType getType() const;
    VkShaderStageFlags getStages() const;
    const string& getName() const;

    //add mentioned stage bits to m_stage
    void addStages(VkShaderStageFlags stage);
    //convert descriptor data to push constant data - keep only name and subset variables
    PushConstantShaderData convertToPushConstant();
    //check whether set is valid
    bool exists() const;
    //convert descriptor data to string
    string str() const;

    bool isUniform() const;
    bool isInputAttachment() const;


    //only valid for input attachment, return pointer to InputAttachmentDescriptorData, return nullptr in case descriptor isn't uniform or storage buffer
    const InputAttachmentDescriptorData* getInputAttachmentData() const;

    const SubsetVariableVector* getSubsetVariables() const;
};

#endif