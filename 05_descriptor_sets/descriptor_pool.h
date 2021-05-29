#ifndef DESCRIPTOR_POOL_H
#define DESCRIPTOR_POOL_H

#include "../00_base/vulkan_base.h"


/**
 * DescriptorPool
 *  - holds a descriptor pool and enables allocating descriptor sets
 *  - is responsible for deleting all descriptor sets that were allocated from it
 */
class DescriptorPool{
    VkDescriptorPool m_descriptor_pool;
    vector<VkDescriptorSet> m_descriptor_sets;
public:
    //create invalid descriptor pool
    DescriptorPool();
    //create descriptor pool from given pool
    DescriptorPool(VkDescriptorPool pool);
    //reset pool and free all allocated sets
    void reset();
    //destroy pool and all sets
    void destroy();

    //Allocate one set from the pool
    VkDescriptorSet allocateSet(const VkDescriptorSetLayout& descriptor_set_layouts);


    //can only be used if pool was created with VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT
    //isn't implemented yet due to unclarities to how indexing should be implemented in final array
    //void freeIndividualSets(int index, int count = 1);
    
    operator const VkDescriptorPool& () const;
    const VkDescriptorPool& get() const;
};

#endif