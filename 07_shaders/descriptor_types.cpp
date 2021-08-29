#include "descriptor_types.h"



const string DescriptorType::shader_descriptor_identifiers[SHADER_DESCRIPTOR_ID_COUNT]{
    "sampler",
    
    "texture1D", "texture2D", "texture3D",
    "itexture1D", "itexture2D", "itexture3D",
    "utexture1D", "utexture2D", "utexture3D",

    "sampler1D", "sampler2D", "sampler3D",
    "isampler1D", "isampler2D", "isampler3D",
    "usampler1D", "usampler2D", "usampler3D",

    "image1D", "image2D", "image3D",
    "iimage1D", "iimage2D", "iimage3D",
    "uimage1D", "uimage2D", "uimage3D",

    "samplerBuffer", "imageBuffer",
    "subpassInput",
    "UNIFORM_BUFFER", "STORAGE_BUFFER", "UNDEFINED"
};
constexpr DescriptorType::DescriptorTypeEnum DescriptorType::descriptor_types[SHADER_DESCRIPTOR_ID_COUNT]{
    TYPE_SAMPLER, 
    
    TYPE_SAMPLED_IMAGE_1D, TYPE_SAMPLED_IMAGE_2D, TYPE_SAMPLED_IMAGE_3D,
    TYPE_SAMPLED_IMAGE_1D, TYPE_SAMPLED_IMAGE_2D, TYPE_SAMPLED_IMAGE_3D,
    TYPE_SAMPLED_IMAGE_1D, TYPE_SAMPLED_IMAGE_2D, TYPE_SAMPLED_IMAGE_3D,
    
    TYPE_COMBINED_IMAGE_1D, TYPE_COMBINED_IMAGE_2D, TYPE_COMBINED_IMAGE_3D,
    TYPE_COMBINED_IMAGE_1D, TYPE_COMBINED_IMAGE_2D, TYPE_COMBINED_IMAGE_3D,
    TYPE_COMBINED_IMAGE_1D, TYPE_COMBINED_IMAGE_2D, TYPE_COMBINED_IMAGE_3D,

    TYPE_STORAGE_IMAGE_1D, TYPE_STORAGE_IMAGE_2D, TYPE_STORAGE_IMAGE_3D,
    TYPE_STORAGE_IMAGE_1D, TYPE_STORAGE_IMAGE_2D, TYPE_STORAGE_IMAGE_3D,
    TYPE_STORAGE_IMAGE_1D, TYPE_STORAGE_IMAGE_2D, TYPE_STORAGE_IMAGE_3D,
    
    TYPE_UNIFORM_TEXEL_BUFFER, TYPE_STORAGE_TEXEL_BUFFER,
    TYPE_INPUT_ATTACHMENT,
    TYPE_UNIFORM_BUFFER, TYPE_STORAGE_BUFFER,
    TYPE_UNDEFINED
};




DescriptorType::DescriptorType() : m_type(TYPE_UNDEFINED)
{}
DescriptorType::DescriptorType(DescriptorTypeEnum type) : m_type(type){}
DescriptorType::DescriptorType(const string& shader_descriptor_id){
    for (uint32_t i = 0; i < sizeof(shader_descriptor_identifiers) / sizeof(shader_descriptor_identifiers[0]); i++){
        if (shader_descriptor_identifiers[i] == shader_descriptor_id){
            m_type = descriptor_types[i];
            return;
        }
    }
    m_type = TYPE_UNDEFINED;
}


const VkDescriptorType DescriptorType::vulkan_descriptor_types[]{
    VK_DESCRIPTOR_TYPE_SAMPLER,

    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,

    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
};
VkDescriptorType DescriptorType::getVulkanDescriptorType() const{
    return vulkan_descriptor_types[(uint32_t) m_type];
}



const string DescriptorType::shader_descriptor_names[]{
    "sampler",
    "gtexture1D", "gtexture2D", "gtexture3D",
    "gsampler1D", "gsampler2D", "gsampler3D",
    "image1D", "image2D", "image3D",
    "samplerBuffer", "imageBuffer",
    "subpassInput",
    "UNIFORM_BUFFER", "STORAGE_BUFFER", "UNDEFINED"
};
string DescriptorType::getString() const{
    return shader_descriptor_names[(uint32_t) m_type];
}
bool DescriptorType::isUndefined() const{
    return m_type == TYPE_UNDEFINED;
}
bool DescriptorType::isUniformBuffer() const{
    return m_type == TYPE_UNIFORM_BUFFER;
}
bool DescriptorType::isStorageBuffer() const{
    return m_type == TYPE_STORAGE_BUFFER;
}
bool DescriptorType::isInputAttachment() const{
    return m_type == TYPE_INPUT_ATTACHMENT;
}
bool DescriptorType::operator==(DescriptorType t2) const{
    return m_type == t2.m_type;
}