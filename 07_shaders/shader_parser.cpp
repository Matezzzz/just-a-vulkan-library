#include "shader_parser.h"
#include "../01_device/allocator.h"




void PushConstantLayout::addPushConstants(const PushConstantShaderData& data, VkShaderStageFlags stage){
    BufferLayoutCreateTypeVector variables;
    //add all variables from given data as push constants
    for (const SubsetVariable& var : data.getVariables()){
        variables.push_back(BufferLayoutCreateType(var.name, var.type, var.count, var.offset));
        m_stage_flags.push_back(stage);
    }
    //update parent MixedBufferLayout object
    *static_cast<MixedBufferLayout*>(this) = variables.create();
}
void PushConstantLayout::combineWith(const PushConstantLayout& p, VkShaderStageFlags stage){
    //go over all push constants to add
    for (const BufferType& t2 : p){
        //if the push constant is over current size, add it without checking(it cannot overlap with any already there)
        if (t2.offset >= m_size){
            push_back(t2, stage);
            continue;
        }
        //whether push constant already exists in another shader stage
        bool exists_already = true;
         
        //whether push constant t2 overlaps any current push constant
        bool overlaps_other = false;

        //go over all push constants currently in the buffer
        for (uint32_t i = 0; i < size(); i++){
            const BufferType& t = (*this)[i];
            //if the two constants are equal
            if (t == t2){
                //add stage to constant stage flags
                exists_already = true;
                m_stage_flags[i] |= stage;
                break;
            }
            //if constant overlaps with one already here, print error and skip adding this one
            if (t.overlaps(t2)){
                PRINT_ERROR("The push constant variables " << t.name << " and " << t2.name << " overlap.")
                overlaps_other = true;
                break;
            }
        }
        //it constant doesn't exist or overlap
        if (!(exists_already || overlaps_other)){
            //if constant is before first one, insert it before all others
            if (t2.end() <= (*this)[0].offset){
                insert(t2, stage, 0);
                continue;
            }
            //go through all constants, if new constant falls between two already there, insert it
            for (uint32_t i = 1; i < size(); i++){
                if ((*this)[i - 1].end() <= t2.offset && (*this)[i].offset >= t2.end()){
                    insert(t2, stage, i);
                    break;
                } 
            }
        }
    }
    //recalculate size with new constants
    recalculateSize();
}
vector<VkPushConstantRange> PushConstantLayout::getPushConstantRanges() const{
    //convert push constants to vector<VkPushConstantRange>
    vector<VkPushConstantRange> ranges;
    ranges.reserve(size());
    for (uint32_t i = 0; i < size(); i++){
        ranges.push_back(VkPushConstantRange{m_stage_flags[i], (*this)[i].offset, roundUpToMemoryBlock((*this)[i].sizeBytes(), 4U)});
    }
    return ranges;
}
VkShaderStageFlags PushConstantLayout::getAllStages() const{
    //get a bitwise or of all stage flags
    VkShaderStageFlags flags = 0;
    for (uint32_t i = 0; i < size(); i++){
        flags |= m_stage_flags[i];
    }
    return flags;
}
void PushConstantLayout::push_back(const BufferType& t, VkShaderStageFlags flags){
    //add t to MixedBufferLayout and matching flags to m_stage_flags
    MixedBufferLayout::push_back(t);
    m_stage_flags.push_back(flags);
}
void PushConstantLayout::insert(const BufferType& t, VkShaderStageFlags flags, uint32_t i){
    //insert values at given positions
    MixedBufferLayout::insert(MixedBufferLayout::begin() + i, t);
    m_stage_flags.insert(m_stage_flags.begin() + i, flags);
}
void PushConstantLayout::recalculateSize(){ 
    //find max end from all push constants
    for (const BufferType& t : (*this)){
        m_size = std::max(t.end(), m_size);
    }
}



void ShaderDataDescriptorSet::addDescriptor(const DescriptorData& descriptor_data){
    //binding = descriptor index in current set
    uint32_t binding = descriptor_data.getBinding();
    //if binding is larger than size, allocate space to fit all descriptors until binding
    if (size() <= binding) resize(binding + 1);
    //save descriptor data
    getDescriptor(binding) = descriptor_data;
}
void ShaderDataDescriptorSet::combineWith(const ShaderDataDescriptorSet& s){
    //source set = s, target set - *this
    //if target set is shared, combine with the shared set
    if (m_shared_set){
        m_shared_set->combineWith(s);
        return;
    }
    //resize target set vector to match binding count of source one
    if (size() < s.size()) resize(s.size());
    //go over all bindings in source set
    for (uint32_t b = 0; b < s.size(); b++){
        //if there is no descriptor in source set in given index, skip this index
        if (!s[b].exists()) continue;
        //if there is no descriptor in target set, copy the one from source set
        if (!getDescriptor(b).exists()){
            getDescriptor(b) = s[b];
            continue;
        }
        //else, check that source and target descriptors are the same and merge their stages, print error if they aren't the same (they overlap)
        if (getDescriptor(b) == s[b]){
            getDescriptor(b).addStages(s[b].getStages());
        }else{
            PRINT_ERROR("Descriptors " << getDescriptor(b).str() << " and " << s[b].str() << " share the same bindings but not other parameters")
        }
    }
}
VkDescriptorSetLayout ShaderDataDescriptorSet::getOrCreateLayout(){
    //return layout of shared set if this set is shared
    if (m_shared_set) return m_shared_set->getOrCreateLayout();

    //return layout if it was created already
    if (m_layout != VK_NULL_HANDLE) return m_layout;
    
    //convert all descriptors from current set into vector<VkDescriptorSetLayoutBinding>
    vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(size());
    for (const DescriptorData& data : (*this)){
        //if there is a descriptor with given binding
        if (data.exists()){
            bindings.push_back(data.getLayoutBinding());
        }
    }
    //fill create info structure
    VkDescriptorSetLayoutCreateInfo create_info{
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0,
        (uint32_t) bindings.size(), bindings.data()
    };
    //create descriptor set layout and return it
    m_layout = g_allocator.get().createDescriptorSetLayout(create_info);
    return m_layout;
}
void ShaderDataDescriptorSet::makeShared(ShaderDataDescriptorSet& d){
    d.combineWith(*this);
    m_shared_set = &d;
    //remove all descriptor data from current set. All functions will now be redirected to the shared set instead
    clear();
}
bool ShaderDataDescriptorSet::isShared() const{
    return m_shared_set;
}
uint32_t ShaderDataDescriptorSet::find(const string& descriptor_name) const{
    //go through all descriptors, return index if one of correct name is found
    for (uint32_t i = 0; i < size(); i++){
        if (getDescriptor(i).getName() == descriptor_name) return i;
    }
    //if name couldn't be found, print error
    PRINT_ERROR("Descriptor of name '" << descriptor_name << "' couldn't be found in the given set")
    return (NPOS_32BIT);
}
UniformBufferLayoutData ShaderDataDescriptorSet::createUniformBufferData(uint32_t b){
    if (!getDescriptor(b).isUniform()){
        PRINT_ERROR("Requesting subset variables data for non-uniform descriptor.")
        return UniformBufferLayoutData();
    }
    //get ptr to subset variables
    const SubsetVariableVector* subset_variables = getDescriptor(b).getSubsetVariables();

    //convert subset variables into vector of variables to create mixed buffer from
    BufferLayoutCreateTypeVector variables;
    for (const SubsetVariable& var : *subset_variables){
        variables.push_back(BufferLayoutCreateType{var.name, var.type, var.count, var.offset});
    }
    //create mixed buffer data from given variables
    return UniformBufferLayoutData(variables.create());
}
UniformBufferLayoutData ShaderDataDescriptorSet::createUniformBufferData(const string& name){
    return createUniformBufferData(find(name));
}
DescriptorData& ShaderDataDescriptorSet::getDescriptor(uint32_t binding_i){
    return (*this)[binding_i];
}
const DescriptorData& ShaderDataDescriptorSet::getDescriptor(uint32_t binding_i) const{
    return (*this)[binding_i];
}



void ShaderDataDescriptorSetVector::addDescriptor(const DescriptorData& descriptor_data){
    uint32_t set = descriptor_data.getSet();
    //if current set count is smaller than the one given descriptor belongs in, resize set vector
    if (size() <= set) resize(set + 1);
    //add descriptor to the set with given index
    getSet(set).addDescriptor(descriptor_data);
}
void ShaderDataDescriptorSetVector::combineWith(const ShaderDataDescriptorSetVector& d){
    //current set vector (*this) = target, d = source set vector
    //if target size is smaller than source, resize target to match 
    if (size() < d.size()) resize(d.size());
    //go over all sets from source
    for (uint32_t s = 0; s < d.size(); s++){
        //if set contains no descriptors, skip it
        if (d[s].size() == 0) continue;
        //if target set contains no descriptors, overwrite it with source one
        if ((*this)[s].size() == 0){
            (*this)[s] = d[s];
            continue;
        }
        //otherwise, combine the two sets
        (*this)[s].combineWith(d[s]);
    }
}
VkDescriptorSetLayout ShaderDataDescriptorSetVector::getOrCreateLayout(uint32_t set_index){
    return getSet(set_index).getOrCreateLayout();
}
void ShaderDataDescriptorSetVector::makeShared(uint32_t set_index, ShaderDataDescriptorSet& set){
    getSet(set_index).makeShared(set);
}
ShaderDataDescriptorSet& ShaderDataDescriptorSetVector::getSet(uint32_t set_i){
    return (*this)[set_i];
}




ShaderDataInfo::ShaderDataInfo() : m_stage(0)
{}
ShaderDataInfo::ShaderDataInfo(const string& glsl_shader_code, VkShaderStageFlags shader_stage) : m_stage(shader_stage){
    parse::string code(glsl_shader_code);
    uint32_t offset = 0;
    parse::string line;
    //while the whole code hasn't been parsed
    while (offset < code.size()){
        //read current line and remove all whitespaces that do not convey information
        line = code.getUntilChar(offset, '\n').cleanRedundantWhitespaces();
        uint32_t line_offset = 0;
        parse::string_view command;
        //read individual commands from line (parts until ;)
        while (line_offset != line.size()){
            //read current command, if it isn't empty, parse it
            command = line.getUntilCharView(line_offset, ';');
            if (command.size()){
                parseShaderLine(command);
            }
        }
    }
}
void ShaderDataInfo::combineWithShaderData(const ShaderDataInfo& info){
    //combine stage flags
    m_stage |= info.m_stage;

    //combine descriptor sets and push constants
    m_descriptor_sets.combineWith(info.m_descriptor_sets);
    m_push_constants.combineWith(info.m_push_constants, info.getStage());
    
    //concantenate both input arrays
    m_inputs.reserve(m_inputs.size() + info.m_inputs.size());
    m_inputs.insert(m_inputs.begin() + m_inputs.size(), info.m_inputs.begin(), info.m_inputs.end());

    //concantenate both output arrays
    m_outputs.reserve(m_outputs.size() + info.m_outputs.size());
    m_outputs.insert(m_outputs.begin() + m_outputs.size(), info.m_outputs.begin(), info.m_outputs.end());
}
VkShaderStageFlags ShaderDataInfo::getStage() const{
    return m_stage;
}
void ShaderDataInfo::makeShared(uint32_t set_i, ShaderDataDescriptorSet& set){
    m_descriptor_sets.makeShared(set_i, set);
}
ShaderDataDescriptorSetVector& ShaderDataInfo::getSets(){
    return m_descriptor_sets;
}
const PushConstantLayout& ShaderDataInfo::getPushConstantLayout() const{
    return m_push_constants;
}
void ShaderDataInfo::parseShaderLine(parse::string_view line){
    uint32_t offset = 0;
    //read first line part
    parse::string_view start = line.getUntilCharView(offset, '(');
    //if first line part is layout, line defines input / output / descriptor
    if (start == "layout"){
        //read all parameters and their values, e.g. location, set, binding, ...
        DescriptorParameterVector parameters {line.getUntilCharView(offset, ')')};
        //get word representing type of line, e.g. in / out / uniform / ...
        parse::string_view info_type {line.getUntilCharView(offset, ' ', true)};
        if (info_type == "flat"){   //if there is a flat qualifier for integers, skip it
            info_type = line.getUntilCharView(offset, ' ', true);
        }


        //get remaining part of command, this is interpreted differently for each command
        parse::string_view type_name_info {line.getSubstrView(offset)};
        if (info_type == "in"){
            //only read vertex shader inputs, other inputs are varying variables between shaders without any CPU interference
            if (m_stage == VK_SHADER_STAGE_VERTEX_BIT){
                readShaderIn(type_name_info, parameters);
            }
        }
        else if (info_type == "out"){
            //only read fragment shader outputs, ignore varying variables
            if (m_stage == VK_SHADER_STAGE_FRAGMENT_BIT){
                readShaderOut(type_name_info, parameters);
            }
        }
        //if type is uniform, read descriptor as a uniform
        else if (info_type == "uniform"){
            readDescriptor(type_name_info, parameters, false);
        }
        //if type is uniform read descriptor as a storage buffer
        else if (info_type == "buffer"){
            readDescriptor(type_name_info, parameters, true);
        }
        //if type isn't any of the above, print error
        else{
            PRINT_ERROR("Invalid variable type found: \'" << info_type.str() << "\'");
        }
    }
    return;
}
void ShaderDataInfo::readDescriptor(parse::string_view name_type, const DescriptorParameterVector& parameters, bool is_storage_buffer){
    bool is_push_constant = parameters.exists("push_constant");
    //create descriptor data from given parameters
    DescriptorData descriptor_data{name_type, parameters, is_storage_buffer, m_stage, is_push_constant};
    //if created descriptor represents a push constant, this can be found in parameters, convert it to push constant
    if (is_push_constant){
        m_push_constants.addPushConstants(descriptor_data.convertToPushConstant(), m_stage);
        return;
    }
    //if not a push constant, add the descriptor
    m_descriptor_sets.addDescriptor(descriptor_data);
}
void ShaderDataInfo::readShaderIn(parse::string_view name_type, const DescriptorParameterVector& parameters){
    m_inputs.push_back(ShaderParseUtils::readShaderInOrOut(name_type, parameters));
}
void ShaderDataInfo::readShaderOut(parse::string_view name_type, const DescriptorParameterVector& parameters){
    m_outputs.push_back(ShaderParseUtils::readShaderInOrOut(name_type, parameters));
}
