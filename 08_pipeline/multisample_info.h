#ifndef MULTISAMPLE_INFO_H
#define MULTISAMPLE_INFO_H

#include "../00_base/vulkan_base.h"

#define SHADING_MASK_FULL 0xFFFFFFFFFFFFFFFF

class MultisampleInfo
{
    uint64_t m_shading_mask;
    VkPipelineMultisampleStateCreateInfo m_info;
public:
    /**
     * Creates a new multisample info for pipeline creation.
     * Default parameters are : 1 sample per fragment, sample shading disabled, no shading mask, alpha to coverage disabled, alpha to one disabled.
     */ 
    MultisampleInfo();

    /**
     * Sets sample count to the given count
     * @param sample_count the count of samples per fragment. Possible values - VK_SAMPLE_COUNT_*** - 1_BIT, 2_BIT, 4_BIT, ..., 64_BIT
     */
    MultisampleInfo& setSampleCount(VkSampleCountFlagBits sample_count);

    /**
     * Enable shading per sample - Running fragment shader for every sample. Needs sampleRateShading feature.
     * @param min_sample_shading minimum fraction of uniquely shaded samples(between 0.0 and 1.0, inclusive)
     */
    MultisampleInfo& enableSampleShading(float min_sample_shading);

    /**
     * Disable shading per sample - run fragment shader only once per fragment
     */
    MultisampleInfo& disableSampleShading();

    /**
     * Set shading mask for shading, logical and is used to get n-th result for n-th sample and n-th bit in the mask
     * @param mask the logical mask to use
     */
    MultisampleInfo& setShadingMask(uint64_t mask);

    /**
     * Fragment's coverage will be generated based on alpha value
     */
    MultisampleInfo& enableAlphaToCoverage();

    /**
     * Fragment's coverage won't be affected by alpha.
     */
    MultisampleInfo& disableAlphaToCoverage();

    /**
     * Alpha component of fragment color will be replaced with a maximum value for given format. Need feature alphaToOne.
     */
    MultisampleInfo& enableAlphaToOne();

    /**
     * Alpha component will be left unaffected.
     */
    MultisampleInfo& disableAlphaToOne();

    /**
     * Return a reference to info object.
     */
    const VkPipelineMultisampleStateCreateInfo* getInfo() const;
};


#endif