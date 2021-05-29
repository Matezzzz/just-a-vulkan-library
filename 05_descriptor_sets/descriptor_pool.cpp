#include "descriptor_pool.h"
#include "../01_device/device.h"

DescriptorPool::DescriptorPool() : m_descriptor_pool(VK_NULL_HANDLE)
{}
DescriptorPool::DescriptorPool(VkDescriptorPool pool) : m_descriptor_pool(pool)
{}
void DescriptorPool::reset(){
    vkResetDescriptorPool(g_device, m_descriptor_pool, 0);
}
void DescriptorPool::destroy(){
    vkDestroyDescriptorPool(g_device, m_descriptor_pool, nullptr);
}
VkDescriptorSet DescriptorPool::allocateSet(const VkDescriptorSetLayout& descriptor_set_layout){
    //fill descriptor set allocate info structure
    VkDescriptorSetAllocateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, m_descriptor_pool, 1, &descriptor_set_layout};
    
    //allocate descriptor set with given layout
    VkDescriptorSet descriptor_set;
    VkResult result = vkAllocateDescriptorSets(g_device, &info, &descriptor_set);
    DEBUG_CHECK("Allocate descriptor set", result)

    //add it to list of descriptor sets allocated from this pool
    m_descriptor_sets.push_back(descriptor_set);
    return descriptor_set;
}
/*
void DescriptorPool::freeIndividualSets(int index, int count)
{
    VkResult result = vkFreeDescriptorSets(m_device, m_descriptor_pool, count, &m_descriptor_sets[index]);
}*/
DescriptorPool::operator const VkDescriptorPool& () const{
    return m_descriptor_pool;
}
const VkDescriptorPool& DescriptorPool::get() const{
    return m_descriptor_pool;
}

