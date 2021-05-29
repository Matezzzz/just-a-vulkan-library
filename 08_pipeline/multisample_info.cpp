#include "multisample_info.h"

//shorthand for 'return *this;'
#define RTRN return *this;

MultisampleInfo::MultisampleInfo() : m_shading_mask(SHADING_MASK_FULL),
    m_info{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0,
        VK_SAMPLE_COUNT_1_BIT, false, 1.f, nullptr, false, false}
{}

MultisampleInfo& MultisampleInfo::setSampleCount(VkSampleCountFlagBits sample_count)
{
    m_info.rasterizationSamples = sample_count; RTRN
}
MultisampleInfo& MultisampleInfo::enableSampleShading(float min_sample_shading)
{
    m_info.sampleShadingEnable = true;
    m_info.minSampleShading = min_sample_shading;
    RTRN
}
MultisampleInfo& MultisampleInfo::disableSampleShading()
{
    m_info.sampleShadingEnable = false; RTRN
}
MultisampleInfo& MultisampleInfo::setShadingMask(uint64_t mask){
    m_shading_mask = mask;
    //if shading mask has all bits set to 1, it can be set to nullptr to achieve the same effect
    if (m_shading_mask == SHADING_MASK_FULL){
        m_info.pSampleMask = nullptr;
    }else{
        //mask is saved as a 64bit value in this class, however, vulkan requires a pointer to start of array of 32bit values. Convert value to a pointer.
        m_info.pSampleMask = (VkSampleMask*) &m_shading_mask;
    }
    RTRN
}
MultisampleInfo& MultisampleInfo::enableAlphaToCoverage()
{
    m_info.alphaToCoverageEnable = true; RTRN
}
MultisampleInfo& MultisampleInfo::disableAlphaToCoverage()
{
    m_info.alphaToCoverageEnable = false; RTRN
}
MultisampleInfo& MultisampleInfo::enableAlphaToOne()
{
    m_info.alphaToOneEnable = true; RTRN
}
MultisampleInfo& MultisampleInfo::disableAlphaToOne()
{
    m_info.alphaToOneEnable = false; RTRN
}
const VkPipelineMultisampleStateCreateInfo* MultisampleInfo::getInfo() const{
    return &m_info;
}

#undef RTRN