#ifndef MIXED_BUFFER_H
#define MIXED_BUFFER_H

#include <stdint.h>
#include <vector>
#include "../07_shaders/shader_types.h"

using std::vector;

//round value size to the next multiple of block_size
uint64_t roundUpToMemoryBlock(uint64_t size, uint64_t block_size);
//round value size to the previous multiple of block size
uint64_t roundDownToMemoryBlock(uint64_t size, uint64_t block_size);


#define OFFSET_NOT_SPECIFIED (~0U)
/**
 * BufferLayoutCreateType
 *  - this class represents one variable for which MixedBufferLayout should be created
 *  - this typically reflects one part of uniform or push constant in shaders
 *  - //! variable alignment is determined using the std430 alignment. This ensures that arrays are tightly packed. However, this seems to be incompatible with SPIR-V alignment, even when specified as std430. Different implementation for arrays might be needed in the future.
 */
class BufferLayoutCreateType{
public:
    //variable name
    string name;
    //variable offset in bytes from start of the structure containing it
    uint32_t offset;
    //how many basic elements does 1 element hold, e.g. 3 elements for vec3
    uint32_t base_length;
    //if the variable is an array, how many elements does it hold
    uint32_t array_length;
    //basic type of variable, can be bool, int, uint, float and double
    ShaderBasicType type;
    //memory alignment required by this type
    uint32_t required_memory_multiple;
    BufferLayoutCreateType(const string& name, ShaderVariableType type, uint32_t array_length = 1, uint32_t offset = OFFSET_NOT_SPECIFIED);
    //return total element count. Equal to (base_length * array_length)
    uint32_t elementCount() const;
};


/**
 * BufferType
 *  - Holds one variable in created MixedBufferLayout
 */
class BufferType{
public:
    //offset in bytes from buffer start
    uint32_t offset;
    //element count - size_in_bytes / sizeof(type)
    uint32_t length;
    //basic type of variable, can be bool, int, uint, float and double
    ShaderBasicType type;
    string name;

    //Create buffer type with given params
    BufferType(uint32_t _offset, uint32_t _length, ShaderBasicType _type, const string& _name);

    //size in bytes
    uint32_t size() const;
    //end of variable in bytes, (offset + size)
    uint32_t end() const;

    //check whether two buffer types are the same
    bool operator==(const BufferType& t2) const;
    //check whether the space occupied of two types overlaps
    bool overlaps(const BufferType& t2) const;
};



/**
 * MixedBufferLayout
 *  - vector of variables that form the layout of a buffer
 *  - mixed buffer represents a byte array on the CPU side, that enables writing variables of multiple types
 *  - this buffer can be then uploaded to the GPU to serve as an uniform buffer or push constants
 */
class MixedBufferLayout : public vector<BufferType>{
protected:
    //total size in bytes
    uint32_t m_size = 0;
public:
    MixedBufferLayout();
    MixedBufferLayout(const vector<BufferType>& variables);
    //find index of variable of given name
    uint32_t find(const string& name);
    uint32_t getSize() const;
    //set size. Should be used only when creating buffer
    void setSize(uint32_t size);
};



/**
 * BufferLayoutCreateTypeVector
 *  - vector of BufferLayoutCreateType variables, these can be used to create a MixedBufferLayout
 */
class BufferLayoutCreateTypeVector : public vector<BufferLayoutCreateType>{
public:
    MixedBufferLayout create(uint32_t first_var_offset = 0);
};



/**
 * MixedBufferData
 *  - this class holds the actual data for the mixed buffer, data can be written by both variable index and name 
 */
class MixedBufferData : public vector<uint8_t>{
    MixedBufferLayout m_variables;
public:
    //create empty data without any variables
    MixedBufferData();
    MixedBufferData(const MixedBufferLayout& variables);
    
    /**
     * Write data to variable with given index.
     * @param var_index index into the internal array of variables to write variable to.
     * @param val pointer to data_len variables of type T. T must be of type bool, int, uint, float or double.
     * @param data_len the number of values of given type to write.
     */
    template<typename T>
    MixedBufferData& write(uint32_t var_index, const T* val, uint32_t data_len){
        ShaderBasicType correct_type = getBasicType(*val);
        if (var_index >= m_variables.size())
        {
            PRINT_ERROR("Could not set mixed buffer data, variable index out of range.");
            return *this;
        }
        const BufferType& buffer_var = m_variables[var_index];
        if (buffer_var.type != correct_type)
        {
            PRINT_ERROR("Attempting to set uniform of incompatible type. Setting " << toString(buffer_var.type) << " with " << toString(correct_type) << " array.\n");
            return *this;
        }
        if (buffer_var.length != data_len)
        {
            PRINT_ERROR("Data to be set is not the same size as available space. Required length: " << buffer_var.length << ", Given data: " << data_len)
            return *this;
        }
        T* ptr = reinterpret_cast<T*>(this->data() + buffer_var.offset);
        memcpy(ptr, val, m_variables[var_index].size());
        return *this;
    }

    /**
     * Write data to variable with given index.
     * @param var_index index into the internal array of variables to write variable to.
     * @param val vector to variables of type T. T must be of type bool, int, uint, float or double.
     */
    template<typename T>
    MixedBufferData& write(uint32_t var_index, const vector<T>& val){
        return write(var_index, val.data(), val.size());
    }

    /**
     * Write data to variable of given name.
     * @param name the name of the variable. For uniform buffers and push constants same as name in shaders.
     * @param val pointer to data_len variables of type T. T must be of type bool, int, uint, float or double.
     * @param data_len the number of values of given type to write.
     */
    template<typename T>
    MixedBufferData& write(const string& name, const T* val, uint32_t data_len){
        uint32_t pos = m_variables.find(name);
        if (pos == ~0U) return *this;
        return write(pos, val, data_len);
    }

    /**
     * Write data to variable with given name.
     * @param name the name of the variable. For uniform buffers and push constants same as name in shaders.
     * @param val vector to variables of type T. T must be of type bool, int, uint, float or double.
     */
    template<typename T>
    MixedBufferData& write(const string& name, const vector<T>& val){
        return write(name, val.data(), val.size());
    }

    /**
     * Write data to variable with given name
     * @param val the value to write
     */
    template<typename T>
    MixedBufferData& write(const string& name, T val){
        return write(name, &val, 1);
    }
};


typedef MixedBufferData UniformBufferData;

#endif