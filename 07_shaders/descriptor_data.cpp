#include "descriptor_data.h"

#include <sstream>


#define INVALID_SET (~0U)



SubsetVariable::SubsetVariable(ShaderVariableType type_, const string& name_, uint32_t count_, uint32_t offset_) : type(type_), name(name_), count(count_), offset(offset_)
{}
bool SubsetVariable::operator==(const SubsetVariable& v) const{
    return (type == v.type && count == v.count && name == v.name && offset == v.offset);
}




bool SubsetVariableVector::operator==(const SubsetVariableVector& d) const{
    //if sizes are different, return false
    if (d.size() != size()) return false;
    //go through all variables and check they are the same, return false if they are not
    for (uint32_t i = 0; i < size(); i++){
        if ((*this)[i] != d[i]) return false;
    }
    return true;
}
string SubsetVariableVector::str() const{
    //put all variables into ostringstream and return the string, example result: "vec4 var1; vec2 var2;"
    std::ostringstream ss;
    for (const SubsetVariable& v : (*this)){
        ss << toString(v.type)  << " " << v.name << "; ";
    }
    return ss.str();
}



PushConstantShaderData::PushConstantShaderData(const string& name, const SubsetVariableVector* variables) : m_name(name), m_variables(*variables)
{}
const string& PushConstantShaderData::getName() const{
    return m_name;
}
const SubsetVariableVector& PushConstantShaderData::getVariables() const{
    return m_variables;
}


DescriptorParameter::DescriptorParameter(const string& name_, uint32_t value_) : name(name_), value(value_)
{}



DescriptorParameterVector::DescriptorParameterVector(parse::string_view parameters){
    //how many characters were parsed already
    uint32_t offset = 0;
    parse::string_view word;
    //while there are parameters to parse. Set word to string part until ',', e.g. "param1=10" from "param1=10,param2"
    while ((word = parameters.getUntilCharView(offset, ',', true)) != ""){
        //find index of '=' char
        uint32_t eq_i = word.find('=');
        //if the parameter has no value, the whole word is its' name
        if (eq_i == NPOS_32BIT){
            push_back(DescriptorParameter(word));
        }else{
            //if the parameter has a value, split word into name and value, convert value to int before creating DescriptorParameter object
            push_back(DescriptorParameter((std::string) word.substr(0, eq_i), readInt(word.getSubstrView(eq_i+1))));
        }
    }
}
uint32_t DescriptorParameterVector::get(const string& name) const{
    //go through all parameters, return param value if it's found
    for (const DescriptorParameter& p : (*this)){
        if (p.name == name){
            return p.value;
        }
    }
    //if no parameter of given name was found
    return PARAMETER_NOT_FOUND;
}
bool DescriptorParameterVector::exists(const string& name) const{
    return (get(name) != PARAMETER_NOT_FOUND);
}
uint32_t DescriptorParameterVector::readInt(parse::string_view v){
    //return PARAMETER_VALUE_NONE if string part to convert is empty
    if (v.size() == 0) return PARAMETER_VALUE_NONE;
    //try converting string to int, print error if conversion fails
    try{
        return std::stoi(string(v));
    }
    catch (std::invalid_argument& e){
        PRINT_ERROR(e.what() << " : Cannot convert value to int. " << v.str())
        return PARAMETER_VALUE_NONE;
    }
}




DescriptorData::DescriptorData() : m_set(INVALID_SET)
{}
DescriptorData::DescriptorData(parse::string_view name_type_info, const DescriptorParameterVector& parameters, bool is_storage_buffer, VkShaderStageFlags stage, bool is_push_constant) : 
    m_set(parameters.get("set")), m_binding(parameters.get("binding")), m_stage(stage)
{
    //current parsing phase. Parameters have been read already, the string is in format 'qualifiers type name', e.g. 'writeonly image2D vel_y_2_out' 
    enum NameTypeReadPhase{
        PHASE_START,        //nothing has been parsed yet
        PHASE_READ_TYPE,    //type (e.g. image2D or vec2) has been read already
        PHASE_READ_NAME     //name has been read already
    } read_phase = PHASE_START;
    //offset in name_type_info
    uint32_t offset = 0;
    //word currently being parsed
    parse::string_view word;
    //while there are words to be read
    while ((word = name_type_info.getUntilCharView(offset, ' ', true)).size()){
        //print error if name has been read already and there are still words left
        if (read_phase == PHASE_READ_NAME){
            PRINT_ERROR("Name has been read already and the input string still continues : " << word.str())
        }
        //if type has been read already, only name and array length is left to read
        if (read_phase == PHASE_READ_TYPE){
            //read array length from given string
            m_count = ShaderParseUtils::readDescriptorCount(word);
            
            uint32_t offset = 0; uint32_t offset2 = 0;
            //get name - part of word before both { and [
            m_name = word.getUntilChar(offset, '[', true).getUntilChar(offset2, '{', true);
            //update read phase
            read_phase = PHASE_READ_NAME;
        }
        if (read_phase == PHASE_START){
            //check whether word is a qualifier
            DescriptorQualifier q = ShaderParseUtils::readDescriptorQualifier(word);
            //if it is, add it to list of qualifiers
            if (q != QUALIFIER_UNRECOGNIZED){
                m_qualifiers.push_back(q);
            }else{
                //if it isn't a qualifier, try reading it as a type
                m_type = descriptorType(word);
                //if it isn't a recognized type, it must be an uniform or storage buffer
                if (m_type == TYPE_UNDEFINED){
                    //choose type between TYPE_STORAGE_BUFFER and TYPE_UNIFORM_BUFFER, this is decided by factors outside of currently parsed string, is given as a parameter
                    m_type = is_storage_buffer ? TYPE_STORAGE_BUFFER : TYPE_UNIFORM_BUFFER;
                    //read subset variables of buffer
                    if (is_push_constant) m_additional_data = new SubsetVariableVector(ShaderParseUtils::readSubsetVariablesData(word));
                    //for uniform and storage buffers, name is part of current word as well, read it
                    uint32_t offset = 0;
                    m_name = word.getUntilChar(offset, '{');
                    //update phase to name having been read
                    read_phase = PHASE_READ_NAME;
                    //set descriptor array length to 1
                    m_count = 1;
                }else{
                    //if type is input attachment, create additional data for it
                    if (m_type == TYPE_INPUT_ATTACHMENT) m_additional_data = ShaderParseUtils::readInputAttachmentData(parameters);
                    //update phase
                    read_phase = PHASE_READ_TYPE;
                }
            }
        }
    }
    //if all data couldn't be read from given string
    if (read_phase != PHASE_READ_NAME) PRINT_ERROR("All information could not be gathered from given string")
}

DescriptorData::DescriptorData(const DescriptorData& d) : m_set(d.m_set), m_binding(d.m_binding), m_stage(d.m_stage), m_type(d.m_type), m_name(d.m_name), m_count(d.m_count){
    //copy additional data based on descriptor type
    if (d.m_additional_data == nullptr){
        m_additional_data = nullptr;
    }
    else if (isInputAttachment()){
        m_additional_data = new InputAttachmentDescriptorData(*d.getInputAttachmentData());
    }else{
        m_additional_data = new SubsetVariableVector(*d.getSubsetVariables());
    }
}
DescriptorData& DescriptorData::operator=(const DescriptorData& d){
    m_set = d.m_set; m_binding = d.m_binding; m_type = d.m_type; m_name = d.m_name; m_count = d.m_count; m_stage = d.m_stage;
    //copy additional data based on descriptor type
    if (d.m_additional_data == nullptr){
        m_additional_data = nullptr;
    }
    else if (d.isInputAttachment()){
        m_additional_data = new InputAttachmentDescriptorData(*d.getInputAttachmentData());
    }else{
        m_additional_data = new SubsetVariableVector(*d.getSubsetVariables());
    }
    return *this;
}
bool DescriptorData::operator==(const DescriptorData& d) const{
    //if type, count, and name are the same
    if (m_type == d.m_type && m_count == d.m_count && m_name == d.m_name){
        //if descriptor is an uniform, check subset variables for equality, else return true
        if (m_additional_data != nullptr && isUniform()){
            if (*getSubsetVariables() == *d.getSubsetVariables()){
                return true;
            }else{
                return false;
            }
        }
        return true;
    }
    return false;
}
DescriptorData::~DescriptorData(){
    //delete additional data based on descriptor type
    if (m_additional_data){
        if (isUniform()){
            delete getSubsetVariables();
        }
        else if (isInputAttachment()){
            delete getInputAttachmentData();
        }
    }
}
string DescriptorData::str() const{
    //convert descriptor data to string, then return it. string should represent legal GLSL code used to instantiate this object.
    std::ostringstream ss;
    ss << "layout(set = " << m_set << ", binding = " << m_binding;
    if (m_type == TYPE_INPUT_ATTACHMENT) ss << ", input_attachment_index = " << getInputAttachmentData()->input_attachment_index;
    ss << ") " << ((m_type == TYPE_STORAGE_BUFFER) ? "buffer" : "uniform");
    ss << " " << descriptorName(m_type);
    if (m_additional_data != nullptr && isUniform()) ss << "{" << getSubsetVariables()->str() << "}";
    ss << " " << m_name << ";";
    return ss.str();
}
bool DescriptorData::isUniform() const {return (m_type == TYPE_UNIFORM_BUFFER || m_type == TYPE_STORAGE_BUFFER);}
bool DescriptorData::isInputAttachment() const {return (m_type == TYPE_INPUT_ATTACHMENT);}
const SubsetVariableVector* DescriptorData::getSubsetVariables() const{
    if (!isUniform()) return nullptr;
    return reinterpret_cast<SubsetVariableVector*>(m_additional_data);
}
const InputAttachmentDescriptorData* DescriptorData::getInputAttachmentData() const{
    if (!isInputAttachment()) return nullptr;
    return reinterpret_cast<InputAttachmentDescriptorData*>(m_additional_data);
}
PushConstantShaderData DescriptorData::convertToPushConstant(){
    //print error if type isn't convertible
    if (m_type != TYPE_UNIFORM_BUFFER) PRINT_ERROR("Converting a different type of descriptor than uniform buffer to push constant. Descriptor name :" << m_name)
    //create push constant data from descriptor name and subset variables
    PushConstantShaderData x{m_name, getSubsetVariables()};
    return x;
}
bool DescriptorData::exists() const{
    return (m_set != INVALID_SET);
}
VkDescriptorSetLayoutBinding DescriptorData::getLayoutBinding() const{
    //convert descriptor to VkDescriptorSetLayoutBinding, this is used for allocating descriptor sets
    return VkDescriptorSetLayoutBinding{m_binding, vulkanDescriptorType(m_type), m_count, m_stage, nullptr};
} 
const string& DescriptorData::getName() const{
    return m_name;
}
uint32_t DescriptorData::getSet() const{
    return m_set;
}
uint32_t DescriptorData::getBinding() const{
    return m_binding;
}
DescriptorType DescriptorData::getType() const{
    return m_type;
}
VkShaderStageFlags DescriptorData::getStages() const{
    return m_stage;
}
void DescriptorData::addStages(VkShaderStageFlags stage){
    m_stage |= stage;
}