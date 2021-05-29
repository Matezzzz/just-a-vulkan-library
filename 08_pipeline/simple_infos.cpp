#include "simple_infos.h"
#include "../01_device/allocator.h"

//shorthand for 'return *this;'
#define RTRN return *this;


VertexInputInfo::VertexInputInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0,
    0, nullptr, 0, nullptr}
{}
VertexInputInfo& VertexInputInfo::addFloatBuffer(vector<uint32_t> data_setup, VkVertexInputRate input_rate){
    //offset in current buffer
    uint32_t offset = 0;
    //one binding for each time this function is called and for each bound buffer on the CPU side
    uint32_t binding = m_buffer_descriptions.size();
    //location in shaders
    uint32_t location = m_data_descriptions.size();
    //byte count per unit of data. Buffers are expected to hold floats.
    uint32_t data_byte_count = sizeof(float);
    //reserve space for data descriptions
    m_data_descriptions.reserve(m_data_descriptions.size() + data_setup.size());
    //for all data parts in current buffer, e.g. (3, 2) for 3 position coords and 2 tex coords
    for (uint32_t length : data_setup){
        //deduce format from data length
        VkFormat format;
        switch(length){
            case 1:
                format = VK_FORMAT_R32_SFLOAT;
                break;
            case 2:
                format = VK_FORMAT_R32G32_SFLOAT;
                break;
            case 3:
                format = VK_FORMAT_R32G32B32_SFLOAT;
                break;
            case 4:
                format = VK_FORMAT_R32G32B32A32_SFLOAT;
                break;
            default:
                PRINT_ERROR("Format of requested size not found");
                break;
        }
        //add data description - defines input from buffer in one location
        //binding = index of buffer to pass data from; offset*data_byte_count = offset in bytes from start of buffer
        m_data_descriptions.push_back({location++, binding, format, offset*data_byte_count});
        //add length to offset, this makes next data start at different index
        offset += length;
    }
    //add buffer description to the list. offset*data_byte_count = stride, input_rate = whether data is specified for each vertex or for each instance
    m_buffer_descriptions.push_back({binding++, offset*data_byte_count, input_rate});
    
    //update internal info structure
    m_info.vertexBindingDescriptionCount = m_buffer_descriptions.size();
    m_info.pVertexBindingDescriptions = m_buffer_descriptions.data();
    m_info.vertexAttributeDescriptionCount = m_data_descriptions.size();
    m_info.pVertexAttributeDescriptions = m_data_descriptions.data();
    RTRN
}
const VkPipelineVertexInputStateCreateInfo* VertexInputInfo::getInfo() const{
    return &m_info;
}



AssemblyInfo::AssemblyInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false}
{}
AssemblyInfo& AssemblyInfo::setTopology(VkPrimitiveTopology topology)
{
    m_info.topology = topology; RTRN
}
AssemblyInfo& AssemblyInfo::enablePrimitiveRestart()
{
    m_info.primitiveRestartEnable = true; RTRN
}
AssemblyInfo& AssemblyInfo::disablePrimitiveRestart()
{
    m_info.primitiveRestartEnable = false; RTRN
}
const VkPipelineInputAssemblyStateCreateInfo* AssemblyInfo::getInfo() const
{
    return &m_info;
}



TesselationInfo::TesselationInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, nullptr, 0,
        0}
{}
TesselationInfo& TesselationInfo::setPointCount(uint32_t count)
{
    m_info.patchControlPoints = count;
    RTRN
}
const VkPipelineTessellationStateCreateInfo* TesselationInfo::getInfo() const
{
    if (m_info.patchControlPoints != 0) return &m_info;
    return nullptr;
}



DynamicInfo::DynamicInfo() : m_dynamic_flags{0}, 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0, 0, nullptr}
{}
DynamicInfo& DynamicInfo::addDynamicState(VkDynamicState dynamic_state){
    m_dynamic_states.push_back(dynamic_state);
        
    m_info.dynamicStateCount = m_dynamic_states.size();
    m_info.pDynamicStates = m_dynamic_states.data();
    RTRN
}
const VkPipelineDynamicStateCreateInfo* DynamicInfo::getInfo() const{
    //if there are any dynamic states defined, return ptr to info, otherwise return nullptr
    if (m_info.dynamicStateCount > 0){
        return &m_info;
    }
    return nullptr;
}



PipelineLayoutInfo::PipelineLayoutInfo() : 
    m_info{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO, nullptr, 0,
        0, nullptr, 0, nullptr}
{}
PipelineLayoutInfo& PipelineLayoutInfo::addDescriptorSetLayout(VkDescriptorSetLayout descriptor_set_layout){
    //add descriptor set layout to the vector, then update pointer and size
    m_descriptor_set_layouts.push_back(descriptor_set_layout);
    m_info.setLayoutCount = m_descriptor_set_layouts.size();
    m_info.pSetLayouts = m_descriptor_set_layouts.data();
    RTRN
}
PipelineLayoutInfo& PipelineLayoutInfo::addPushConstant(const VkPushConstantRange& push_constant){
    //add push constant range to the vector, then update pointer and size
    m_push_constants.push_back(push_constant);
    m_info.pushConstantRangeCount = m_push_constants.size();
    m_info.pPushConstantRanges = m_push_constants.data();
    RTRN
}
PipelineLayoutInfo& PipelineLayoutInfo::setPushConstants(const vector<VkPushConstantRange>& push_constants){
    //set multiple push constants at once, then update pointer and size
    m_push_constants = push_constants;
    m_info.pushConstantRangeCount = m_push_constants.size();
    m_info.pPushConstantRanges = m_push_constants.data();
    RTRN
}
VkPipelineLayout PipelineLayoutInfo::create() const{
    return g_allocator.get().createPipelineLayout(m_info);
}

#undef RTRN