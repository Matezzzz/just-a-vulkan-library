#ifndef DESCRIPTOR_TYPES_H
#define DESCRIPTOR_TYPES_H

#include "../00_base/extension_utilities.h"


//all descriptor types - some types represent a class of descriptors - e.g IMAGE_2D can be 'texture2D', 'itexture2D' and 'utexture2D' in shaders
enum DescriptorType{
    TYPE_SAMPLER, 
    TYPE_SAMPLED_IMAGE_1D, TYPE_SAMPLED_IMAGE_2D, TYPE_SAMPLED_IMAGE_3D,
    TYPE_COMBINED_IMAGE_1D, TYPE_COMBINED_IMAGE_2D, TYPE_COMBINED_IMAGE_3D,
    TYPE_STORAGE_IMAGE_1D, TYPE_STORAGE_IMAGE_2D, TYPE_STORAGE_IMAGE_3D,
    TYPE_UNIFORM_TEXEL_BUFFER, TYPE_STORAGE_TEXEL_BUFFER,
    TYPE_INPUT_ATTACHMENT,
    TYPE_UNIFORM_BUFFER, TYPE_STORAGE_BUFFER,
    TYPE_UNDEFINED
};
//names of each above mentioned descriptor type
constexpr const char* shader_descriptor_names[]{
    "sampler",
    "gtexture1D", "gtexture2D", "gtexture3D",
    "gsampler1D", "gsampler2D", "gsampler3D",
    "image1D", "image2D", "image3D",
    "samplerBuffer", "imageBuffer",
    "subpassInput",
    "UNIFORM_BUFFER", "STORAGE_BUFFER", "UNDEFINED"
};
//all descriptor type names that are used in shaders
const string shader_descriptor_identifiers[]{
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
//used to convert shader descriptor identifier to DescriptorType
constexpr DescriptorType descriptor_types[]{
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
//used to convert DescriptorType to vulkan descriptor types
constexpr VkDescriptorType vulkan_descriptor_types[]{
    VK_DESCRIPTOR_TYPE_SAMPLER,

    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,

    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
};
//convert identifier to descriptorType
constexpr DescriptorType descriptorType(const string& identifier){
    for (uint32_t i = 0; i < sizeof(shader_descriptor_identifiers) / sizeof(shader_descriptor_identifiers[0]); i++){
        if (shader_descriptor_identifiers[i] == identifier) return descriptor_types[i];
    }
    return TYPE_UNDEFINED;
}
//convert descriptor type to vulkan descriptor type
constexpr VkDescriptorType vulkanDescriptorType(DescriptorType type){
    return vulkan_descriptor_types[(uint32_t) type];
}
//convert descriptor type to string
constexpr const char* descriptorName(DescriptorType type){
    return shader_descriptor_names[(uint32_t) type];
}

#endif