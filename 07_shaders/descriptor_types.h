#ifndef DESCRIPTOR_TYPES_H
#define DESCRIPTOR_TYPES_H

#include "../00_base/extension_utilities.h"


//all descriptor types - some types represent a class of descriptors - e.g IMAGE_2D can be 'texture2D', 'itexture2D' and 'utexture2D' in shaders
class DescriptorType{
public:
    enum DescriptorTypeEnum{
        TYPE_SAMPLER, 
        TYPE_SAMPLED_IMAGE_1D, TYPE_SAMPLED_IMAGE_2D, TYPE_SAMPLED_IMAGE_3D,
        TYPE_COMBINED_IMAGE_1D, TYPE_COMBINED_IMAGE_2D, TYPE_COMBINED_IMAGE_3D,
        TYPE_STORAGE_IMAGE_1D, TYPE_STORAGE_IMAGE_2D, TYPE_STORAGE_IMAGE_3D,
        TYPE_UNIFORM_TEXEL_BUFFER, TYPE_STORAGE_TEXEL_BUFFER,
        TYPE_INPUT_ATTACHMENT,
        TYPE_UNIFORM_BUFFER, TYPE_STORAGE_BUFFER,
        DESCRIPTOR_TYPE_COUNT,
        TYPE_UNDEFINED
    };
private:
    DescriptorTypeEnum m_type;


    static const int SHADER_DESCRIPTOR_ID_COUNT = 34;
    //all descriptor type names that are used in shaders
    static const string shader_descriptor_identifiers[SHADER_DESCRIPTOR_ID_COUNT];
    //used to convert shader descriptor identifier to DescriptorType
    static const DescriptorTypeEnum descriptor_types[SHADER_DESCRIPTOR_ID_COUNT];
    
    //used to convert DescriptorType to vulkan descriptor types
    static const VkDescriptorType vulkan_descriptor_types[];

    //names of each above mentioned descriptor type
    static const string shader_descriptor_names[];

public:
    DescriptorType();
    DescriptorType(DescriptorTypeEnum type);
    DescriptorType(const string& shader_descriptor_id);
    VkDescriptorType getVulkanDescriptorType() const;
    string getString() const;
    bool isUndefined() const;
    bool isUniformBuffer() const;
    bool isStorageBuffer() const;
    bool isInputAttachment() const;

    bool operator==(DescriptorType t2) const;
};


#endif