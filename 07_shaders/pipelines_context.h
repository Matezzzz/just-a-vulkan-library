#ifndef PIPELINES_CONTEXT_H
#define PIPELINES_CONTEXT_H

#include "../01_device/device.h"
#include "../05_descriptor_sets/descriptor_pool.h"
#include "shader_parser.h"
#include <map>

using std::map;

class Pipeline;
class PipelineInfo;



enum DescriptorUpdateInfoType{
    UPDATE_INFO_IMAGE,
    UPDATE_INFO_BUFFER
};

/**
 * DescriptorUpdateInfo
 *  - Holds all information for updating a descriptor.
 */
class DescriptorUpdateInfo{
    string m_name;
    
    //whether descipror is image or buffer
    DescriptorUpdateInfoType m_type;

    //VkDescriptorImageInfo or VkDescriptorBufferInfo based on descriptor type
    void* m_info = nullptr;

    VkDescriptorType m_vulkan_type;
public:
    //constructor for image and combined image and sampler
    DescriptorUpdateInfo(const string& name, VkDescriptorType type, VkImageLayout layout, VkImageView image, VkSampler sampler);

    DescriptorUpdateInfo(const DescriptorUpdateInfo& i);
    DescriptorUpdateInfo& operator=(const DescriptorUpdateInfo& i);

    //constructor for buffer variables
    DescriptorUpdateInfo(const string& name, VkDescriptorType type, VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
    ~DescriptorUpdateInfo();

    bool isImage() const;
    const string& getName() const;
    VkDescriptorType getType() const;

    //return image info if descriptor being updated is an image, otherwise return nullptr
    const VkDescriptorImageInfo* imageInfo() const;

    //return buffer info if descriptor being updated is a buffer, otherwise return nullptr
    const VkDescriptorBufferInfo* bufferInfo() const;
private:
    //create a copy of m_info and return a pointer to it
    void* copyInfo() const;
};

/**
 * SamplerUpdateInfo
 *  - special DescriptorUpdateInfo subclass for samplers
 */
class SamplerUpdateInfo : public DescriptorUpdateInfo{
public:
    SamplerUpdateInfo(const string& name, VkSampler sampler);
};

/**
 * CombinedImageSamplerUpdateInfo
 *  - special DescriptorUpdateInfo subclass for combined image samplers
 */
class CombinedImageSamplerUpdateInfo : public DescriptorUpdateInfo{
public:
    CombinedImageSamplerUpdateInfo(const string& name, VkImageView image, VkSampler sampler, VkImageLayout layout);
};
typedef CombinedImageSamplerUpdateInfo TextureUpdateInfo;

/**
 * SampledImageUpdateInfo
 *  - special DescriptorUpdateInfo subclass for sampled images
 */
class SampledImageUpdateInfo : public DescriptorUpdateInfo{
public:
    SampledImageUpdateInfo(const string& name, VkImageView image, VkImageLayout layout);
};

/**
 * StorageImageUpdateInfo
 *  - special DescriptorUpdateInfo subclass for storage images
 */
class StorageImageUpdateInfo : public DescriptorUpdateInfo{
public:
    StorageImageUpdateInfo(const string& name, VkImageView image, VkImageLayout layout);
};


/**
 * UniformBufferUpdateInfo
 *  - special DescriptorUpdateInfo subclass for uniform buffers
 */
class UniformBufferUpdateInfo : public DescriptorUpdateInfo{
public:
    UniformBufferUpdateInfo(const string& name, VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
};



/**
 * StorageBufferUpdateInfo
 *  - special DescriptorUpdateInfo subclass for storage buffers
 */
class StorageBufferUpdateInfo : public DescriptorUpdateInfo{
public:
    StorageBufferUpdateInfo(const string& name, VkBuffer buffer, VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
};



/**
 * ShaderStages
 *  - Holds shader stages created by vulkan. Holds no information about descriptors.
 */
class ShaderStages : public vector<VkPipelineShaderStageCreateInfo>{
public:
    //Create shader stage objects for given shaders
    ShaderStages(const vector<ShaderData>& shaders);

    //return a reference to first shader, print error if there are more or if the stage isn't compute. 
    const VkPipelineShaderStageCreateInfo& getCompute() const;
};



/**
 * DescriptorSet
 *  - Holds one descriptor set handle, enables descriptor set updating. Holds a poitner to descriptor set layout - requires parent pipeline context to exist
 */
class DescriptorSet{
    VkDescriptorSet m_set;
    //the pointer to parent layout - this contains names of descriptors in the set, their types, ...
    const ShaderDataDescriptorSet* m_layout;
public:
    //create invalid descriptor set
    DescriptorSet();
    //create descriptor set for the given set with given layout
    DescriptorSet(VkDescriptorSet set, const ShaderDataDescriptorSet& m_layout);
    operator VkDescriptorSet();

    //Pass any number of DescriptorUpdateInfo const references that will update sets
    template<typename ...Ts>
    void updateDescriptors(const Ts&... update_infos){
        //allocate vector of vulkan write infos
        vector<VkWriteDescriptorSet> sets(sizeof...(Ts));
        //call internal update function to fill all write infos and eventually update them
        updateDescriptorsInternal(sets, 0, update_infos...);
    }

    //update descriptors using given vector of infos (update descriptors vector)
    void updateDescriptorsV(const vector<DescriptorUpdateInfo>& infos){
        vector<VkWriteDescriptorSet> writes(infos.size());
        //convert infos to VkWriteDescriptorSet structures
        for (int i = 0; i < (int) infos.size(); i++){
            saveDescriptorWriteInfo(writes[i], infos[i]);
        }
        //update descriptor sets
        updateDescriptorSets(writes);
    }
    //update all descriptors using vkUpdateDescriptorSets function
    void updateDescriptorSets(const vector<VkWriteDescriptorSet>& write_infos){
        vkUpdateDescriptorSets(g_device, write_infos.size(), write_infos.data(), 0, nullptr);
    }
private:
    //if there are no more infos to convert to VkWriteDescriptorSet objects, update sets 
    void updateDescriptorsInternal(vector<VkWriteDescriptorSet>& write_infos, uint32_t){
        updateDescriptorSets(write_infos);
    }

    //fill write info at current_index with information from info, then call updateDescriptorsInternal again with update infos
    template<typename ...Ts>
    void updateDescriptorsInternal(vector<VkWriteDescriptorSet>& write_infos, uint32_t current_index, const DescriptorUpdateInfo& info, const Ts&... update_infos)
    {
        saveDescriptorWriteInfo(write_infos[current_index], info);
        //call the template function to update following write infos
        updateDescriptorsInternal(write_infos, ++current_index, update_infos...);
    }

    //convert one write info to VkWriteDescriptorSet object and save it to given reference
    void saveDescriptorWriteInfo(VkWriteDescriptorSet& write_info, const DescriptorUpdateInfo& info){
        uint32_t descriptor_index = m_layout->find(info.getName());
        if (descriptor_index != NPOS_32BIT){
            //check whether shader descriptor type is of the same type as update info
            VkDescriptorType shader_descriptor_type = vulkanDescriptorType((*m_layout)[descriptor_index].getType());
            if (shader_descriptor_type == info.getType()){
                //fill VkWriteDescriptorSet structure
                write_info = VkWriteDescriptorSet{
                    VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    nullptr, m_set, descriptor_index, 0,
                    1, shader_descriptor_type,
                    info.imageInfo(), info.bufferInfo(), nullptr
                };
            }else{
                PRINT_ERROR(info.getName() << ": Attempt to write descriptor of invalid type")
            }
        }else{
            PRINT_ERROR(info.getName() << ": Descriptor of given name not found")
        }
    }
};



/**
 * DescriptorSetCounter
 *  - counts how many times does each type of descriptor set apperar within the descriptor pool.
 */
class DescriptorSetCounter{
    uint32_t m_descriptor_counts[11]{};
public:
    //add given count of descriptor type represented by data
    void addDescriptors(const DescriptorData& data, uint32_t descriptor_count);

    //return pool sizes required to create the pool - how many descriptors of each type are needed
    vector<VkDescriptorPoolSize> getPoolSizes() const;
};



/**
 * DescriptorSetManager
 *  - Responsible for counting needed descriptor sets, then creating pool for them, then allocating them.
 */
class DescriptorSetManager{
    //how many sets can be allocated
    uint32_t m_set_count = 0;
    //count number of occurences of each descriptor type
    DescriptorSetCounter m_set_counter;
    //the pool to allocate from
    DescriptorPool m_descriptor_pool;
public:
    //reserve given count of set with given layout
    void reserveSets(const ShaderDataDescriptorSet& set, uint32_t set_count);

    //create descriptor pool. Should be called after all sets were reserved.
    void createPool();

    //allocate descriptor set with given layout. Should be called after creating descriptor pool.
    VkDescriptorSet allocateSet(VkDescriptorSetLayout layout);
};



/**
 * SetVector
 *  - Convert a list of DescriptorSets to a vector of pointers to them
 */
class SetVector : public vector<DescriptorSet*>{
public:
    //constructor - pass a list of DescriptorSet references
    template<typename ...Ts>
    SetVector(Ts&... sets);
private:
    //additional functions used for initialization
    void initFunc(){}
    template<typename ...Ts>
    void initFunc(DescriptorSet& s, Ts&... sets){
        //add current pointer to the list
        push_back(&s);
        //add remaining pointers
        initFunc(sets...);
    }
};



/**
 * PushConstantData
 *  - Holds data for push constants. These can be written, then given to the gpu by calling cmdPushConstants on a command buffer
 */
class PushConstantData : public MixedBufferData{
    //stages, in which constants are used
    VkShaderStageFlags m_stages;
public:
    PushConstantData(const PushConstantLayout& layout);
    VkShaderStageFlags getStages() const;
};


typedef MixedBufferData UniformBufferData;


const uint32_t DESCRIPTOR_SET_SKIP = ~0U;
/**
 * PipelineContext
 *  - This class is responsible for loading and holding all information related to one shader.
 */ 
class PipelineContext{
    //holds vulkan compiled shader units
    const ShaderStages m_shader_stages;
    //Holds information about all descriptors, inputs and outputs
    ShaderDataInfo m_shader_info;
    //Holds handle of a pipeline layout, if it has been created already
    VkPipelineLayout m_pipeline_layout;
    //Reference to parent context descriptor set manager. One manager exists for one collection of shaders.
    DescriptorSetManager& m_set_manager;
    //counts of descriptor sets used by this shader. Are specified using the reserveDescriptorSets function
    vector<uint32_t> m_descriptor_set_counts;
public:
    //Create context given shaders and descriptor set manager
    PipelineContext(const ShaderDirectoryData& shader_dir_data, DescriptorSetManager& descriptor_set_manager);
    
    //make descriptor set of given index shared - All descriptors have to be set only once for both sets, and it's allocated only once
    //! not tested yet, might not work
    void makeSharedDescriptorSet(uint32_t set_i_1, PipelineContext& ctx, uint32_t set_i_2);

    //make push constants shared between two pipeline contexts
    void makeSharedPushConstantLayout(PipelineContext& ctx);
    
    //Create push constant data to be written using current constant layout
    PushConstantData createPushConstantData();

    /**
     * Create uniform buffer data to be written. This data can be then copied to the GPU
     * @param set_i index of the set to search for descriptor of given name in
     * @param descriptor_name name of descriptor to get data of.
     */
    UniformBufferData createUniformBufferData(uint32_t set_i, const string& descriptor_name);

    /**
     * Create graphics pipeline
     * @param info information about pipeline parameters
     * @param render_pass the render pass, in which the pipeline will be used
     * @param subpass_index the subpass index of the above mentioned render pass, where the pipeline will be used, can be omitted to be 0
     */
    Pipeline createPipeline(const PipelineInfo& info, VkRenderPass render_pass, uint32_t subpass_index = 0);

    //Create compute pipeline
    Pipeline createComputePipeline();
    
    /**
     * Reserve given given count of each descriptor set. There has to be same amount of counts and descriptor sets present in shaders of this context.
     * @param counts multiple unsigned integers, each representing set count of given index
     */
    template<typename ...Ts>
    void reserveDescriptorSets(Ts... counts)
    {
        //number of sets needed by shaders
        uint32_t set_count = m_shader_info.getSets().size();
        //check that there is same number of shader sets and numbers given
        if (sizeof...(Ts) != set_count){
            PRINT_ERROR("Incorrect number of descriptor set counts to reserve. Required: " << set_count << ", given: " << sizeof...(Ts))
            return;
        }
        //resize set counts to reflect amount of numbers
        m_descriptor_set_counts.resize(sizeof...(Ts), 0);
        //
        reserveDescriptorSetsInternal(0, counts...);
    }

    /**
     * Allocate descriptor sets into structures passed.
     * @param sets the sets to write allocated sets into. Each element has to reflect amount of reserved sets, and there has to be same amount of variables as sets. Pass DESCRIPTOR_SET_SKIP to skip a set, DescriptorData& to allocate one set, and SetVector to allocate multiple sets
     */
    template<typename ...Ts>
    void allocateDescriptorSets(Ts&... sets){
        //make sure are as many set structures as sets
        uint32_t set_count = m_shader_info.getSets().size();
        if (sizeof...(Ts) != set_count){
            PRINT_ERROR("Incorrect number of descriptor set counts to allocate. Required: " << set_count << ", given: " << sizeof...(Ts))
            return;
        }
        //call internal function to allocate all sets into given structures
        allocateDescriptorSetsInternal(0, sets...);
    }
private:
    //Get a reference to set of given index
    ShaderDataDescriptorSet& getDescriptorSet(uint32_t i);

    //Create pipeline layout if it hasn't been created yet
    void keepOrCreatePipelineLayout();

    
    /**
     * Reserve descriptor sets.
     * @param cur_index index of the set to reserve
     * @param count how many sets should be reserved
     * @param counts the counts of reservations for next sets
     */
    template<typename ...Ts>
    void reserveDescriptorSetsInternal(uint32_t cur_index, uint32_t count, Ts... counts){
        //save descriptor set count
        m_descriptor_set_counts[cur_index] = count;
        //reserve sets in descriptor set manager
        m_set_manager.reserveSets(m_shader_info.getSets()[cur_index], count);
        //call recursive function to reserve remaining sets
        reserveDescriptorSetsInternal(++cur_index, counts...);
    }
    //Called when there are no more sets to reserve. Do nothing.
    void reserveDescriptorSetsInternal(uint32_t){}

    


    /**
     * Allocate descriptor set function - when current set structure is of type uint, it means that current set index is not used in shaders and should be skipped.
     * @param cur_index index of set being skipped
     * @param set_skip_n any uint to signify skipped set
     * @param other_sets other structures to allocate sets into
     */
    template<typename ...Ts>
    void allocateDescriptorSetsInternal(uint32_t cur_index, uint32_t& set_skip_n, Ts&... other_sets){
        //get available set count
        uint32_t set_count = m_descriptor_set_counts[cur_index];
        //if there are reserved sets that the user is not asking for, print error
        if (set_count != 0) PRINT_ERROR("Not asking for allocation of a reserved set. Index: " << cur_index <<", Available sets: " << set_count);
        //allocate remaining sets
        allocateDescriptorSetsInternal(++cur_index, other_sets...);
    }

    /**
     * Allocate descriptor set function - when current set structure is of type DescriptorSet&, it means that user is requesting a single descriptor set
     * @param cur_index index of set being allocated
     * @param set set to be set when allocated
     * @param other_sets other structures to allocate sets into
     */
    template<typename ...Ts>
    void allocateDescriptorSetsInternal(uint32_t cur_index, DescriptorSet& set, Ts&... other_sets){
        //current number of reserved sets
        uint32_t set_count = m_descriptor_set_counts[cur_index];
        //if user is not requesting one set exactly, print error
        if (set_count != 1) PRINT_ERROR("Requesting allocation of invalid number of sets. Set index: " << cur_index << ", Available: " << set_count << ", Requesting: 1");
        //allocate one set of correct layout, then save it into set structure
        set = DescriptorSet(m_set_manager.allocateSet(m_shader_info.getSets()[cur_index].getOrCreateLayout()), m_shader_info.getSets()[cur_index]);
        //allocate remaining sets
        allocateDescriptorSetsInternal(++cur_index, other_sets...);
    }

    /**
     * Allocate descriptor set function - when current set structure is of type SetVector&, it means that user is requesting 'sets.size()' sets
     * @param cur_index index of set being allocated
     * @param sets sets to be set when allocated
     * @param other_sets other structures to allocate sets into
     * !this function is not tested yet, might not work
     */
    template<typename ...Ts>
    void allocateDescriptorSetsInternal(uint32_t cur_index, SetVector& sets, Ts&... other_sets){
        //current number of reserved sets
        uint32_t set_count = m_descriptor_set_counts[cur_index];
        //if current number of sets is not the same as reserved set count
        if (set_count != sets.size()) PRINT_ERROR("Requesting allocation of invalid number of sets. Set index: " << cur_index << ", Available: " << set_count << ", Requesting: " << sets.size());
        //here sets should be allocated and saved
        for (DescriptorSet* s : sets){
            *s = DescriptorSet(m_set_manager.allocateSet(m_shader_info.getSets()[cur_index].getOrCreateLayout()), m_shader_info.getSets()[cur_index]);
        }
        //allocate remaining sets
        allocateDescriptorSetsInternal(++cur_index, other_sets...);
    }
    //Called when there are no more sets to allocate. Do nothing.
    void allocateDescriptorSetsInternal(uint32_t){}
};




/**
 * DirectoryPipelinesContext
 *  - holds all pipeline contexts for the given directory
 */
class DirectoryPipelinesContext{
    //holds descriptor pool and all allocated sets for all pipeline contexts
    DescriptorSetManager m_descriptor_set_manager;
    //hold all pipeline contexts, indexable by name
    map<string, PipelineContext> m_pipeline_contexts;
public:
    //load pipeline contexts from given directory
    DirectoryPipelinesContext(const string& directory);
    //get context of given name, print error and return invalid handle if it doesn't exist
    PipelineContext& getContext(const string& name);
    //create descriptor pool. All sets have to be reserved before this is called.
    void createDescriptorPool();
};

#endif