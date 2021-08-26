#ifndef SAMPLER_H
#define SAMPLER_H

#include "../00_base/vulkan_base.h"
#include "../01_device/allocator.h"


/**
 * SamplerInfo
 *  - Holds settings for creating a sampler
 *  - Sampler is an object that controls how textures are read in shaders
 */
class SamplerInfo{
    VkSamplerCreateInfo m_info;
public:
    //create vulkan sampler info with default parameters, those being: Nearest filtering min, mag, mipmap; repeat wrap in all direcions; 0 LOD bias and clamping; anisotropy disabled, max 0; comparison disabled; border color transparent black; coordinates normalized. 
    SamplerInfo();
    //sets filters when zooming in on or zooming out on texture, possibilities - VK_FILTER_NEAREST or VK_FILTER_LINEAR
    SamplerInfo& setFilters(VkFilter min_filter, VkFilter mag_filter);
    //sets filters for accesing mipmaps, possibilities - VK_SAMPLER_MIPMAP_MODE_NEAREST or VK_SAMPLER_MIPMAP_MODE_LINEAR
    SamplerInfo& setMipmapFilterMode(VkSamplerMipmapMode mipmap_mode);
    //Sets action taken for any texture coordinates out of range <0, 1>. Possibilities: VK_SAMPLER_MODE_REPEAT, VK_SAMPLER_MODE_MIRRORED_REPEAT, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, or VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
    SamplerInfo& setWrapMode(VkSamplerAddressMode wrap_mode);
    //Sets action for X texture coordinates out of range <0, 1>. Possibilities: VK_SAMPLER_ADRESS_MODE_*** - REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, or MIRROR_CLAMP_TO_EDGE
    SamplerInfo& setWrapModeX(VkSamplerAddressMode wrap_mode);
    //Sets action for X texture coordinates out of range <0, 1>. Possibilities: VK_SAMPLER_ADRESS_MODE_*** - REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, or MIRROR_CLAMP_TO_EDGE
    SamplerInfo& setWrapModeY(VkSamplerAddressMode wrap_mode);
    //Sets action for X texture coordinates out of range <0, 1>. Possibilities: VK_SAMPLER_ADRESS_MODE_*** - REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER, or MIRROR_CLAMP_TO_EDGE   
    SamplerInfo& setWrapModeZ(VkSamplerAddressMode wrap_mode);

    /* THE FOLLOWING MAY BE REQUIRED TO BE IMPLEMENTED IN THE FUTURE. CURRENTLY THEY ARE NOT NEEDED.
    SamplerInfo& setMipmapBias(float bias);
    SamplerInfo& enableAnisotropy();
    SamplerInfo& disableAnisotropy();
    SamplerInfo& enableCompare(VkCompareOp compare_operation);
    SamplerInfo& disableCompare();
    SamplerInfo& setLODRange(float min_lod, float max_lod);*/
    
    //sets border color when using CLAMP TO BORDER wrap mode. Possibilities: VK_BORDER_COLOR_*** - FLOAT_TRANSPARENT_BLACK _INT_TRANSPARENT_BLACK, FLOAT_OPAQUE_BLACK, INT_OPAQUE_BLACK, FLOAT_OPAQUE_WHITE, or INT_OPAQUE_WHITE
    SamplerInfo& setBorderColor(VkBorderColor color);
    //texture coordinates will range from 0 to 1 instead of the actual image dimensions
    SamplerInfo& enableNormalizedCoordinates();
    //texture coordinates will range from 0 to size of the image
    SamplerInfo& disableNormalizedCoordinates();

    VkSampler create() const;
};

#endif