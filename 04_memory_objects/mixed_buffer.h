#ifndef MIXED_BUFFER_H
#define MIXED_BUFFER_H

#include <stdint.h>
#include <vector>
#include "../07_shaders/shader_types.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::vector;


//round value size to the previous multiple of block size
template<typename T>
T roundDownToMemoryBlock(T size, T block_size){
    //(size % block_size) computes offset from current block start
    //subtract offset from size and return
    return size - (size % block_size);
}

//round value size to the next multiple of block_size
template<typename T>
T roundUpToMemoryBlock(T size, T block_size){
    //compute value surely in the next block
    T a = size + block_size - 1;
    //round value to the start of block - start of next block has to be end of current one
    return roundDownToMemoryBlock(a, block_size);
}



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

    //size in elements
    uint32_t size() const;
    //size in bytes
    uint32_t sizeBytes() const;
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
 * UniformBufferData
 *  - this class holds the actual data for the uniform buffer
 */
class UniformBufferData : public vector<uint8_t>{
public:
    UniformBufferData(uint32_t buffer_size = 0) : vector<uint8_t>(buffer_size, 0)
    {}

    template<typename T>
    UniformBufferData& write(uint32_t data_offset_bytes, const T* val, uint32_t data_len_elements){
        writeBytes(data_offset_bytes, val, data_len_elements * sizeof(T));
        return *this;
    }
    void writeBytes(uint32_t data_offset_bytes, const void* val, uint32_t data_len_bytes){
        if (data_offset_bytes + data_len_bytes >= size()){
            resize(data_offset_bytes + data_len_bytes);
        }
        memcpy(data() + data_offset_bytes, val, data_len_bytes);
    }
};



class UniformBufferRawData : public UniformBufferData{
protected:
    uint32_t m_write_offset;
public:
    template<typename T>
    UniformBufferRawData& write(const T* val, uint32_t data_len_elements){
        UniformBufferData::write(m_write_offset, val, data_len_elements);
        m_write_offset += data_len_elements * sizeof(T);
        return *this;
    }
    template<typename T, uint32_t data_len_elements>
    UniformBufferRawData& write(const T* val){
        return write(val, data_len_elements);
    }
};


vector<uint32_t> convertArrayOfBools(uint32_t len, const bool* data);


class UniformBufferRawDataSTD140 : public UniformBufferRawData{
public:
    template<typename T, uint32_t data_len_elements, uint32_t alignment>
    UniformBufferRawDataSTD140& write(const T* val){
        m_write_offset = roundUpToMemoryBlock(m_write_offset, alignment);
        UniformBufferRawData::write<T, data_len_elements>(val);
        return *this;
    }

    UniformBufferRawDataSTD140& write(bool     n);
    UniformBufferRawDataSTD140& write(int32_t  n);
    UniformBufferRawDataSTD140& write(uint32_t n);
    UniformBufferRawDataSTD140& write(float    n);
    UniformBufferRawDataSTD140& write(double   n);

    UniformBufferRawDataSTD140& writeBVec2(const bool* data);
    UniformBufferRawDataSTD140& writeBVec3(const bool* data);
    UniformBufferRawDataSTD140& writeBVec4(const bool* data);
    UniformBufferRawDataSTD140& write(const glm::bvec2& data);
    UniformBufferRawDataSTD140& write(const glm::bvec3& data);
    UniformBufferRawDataSTD140& write(const glm::bvec4& data);

    UniformBufferRawDataSTD140& writeIVec2(const int32_t* data);
    UniformBufferRawDataSTD140& writeIVec3(const int32_t* data);
    UniformBufferRawDataSTD140& writeIVec4(const int32_t* data);
    UniformBufferRawDataSTD140& write(const glm::ivec2& data);
    UniformBufferRawDataSTD140& write(const glm::ivec3& data);
    UniformBufferRawDataSTD140& write(const glm::ivec4& data);

    UniformBufferRawDataSTD140& writeUVec2(const uint32_t* data);
    UniformBufferRawDataSTD140& writeUVec3(const uint32_t* data);
    UniformBufferRawDataSTD140& writeUVec4(const uint32_t* data);
    UniformBufferRawDataSTD140& write(const glm::uvec2& data);
    UniformBufferRawDataSTD140& write(const glm::uvec3& data);
    UniformBufferRawDataSTD140& write(const glm::uvec4& data);

    UniformBufferRawDataSTD140& writeVec2(const float* data);
    UniformBufferRawDataSTD140& writeVec3(const float* data);
    UniformBufferRawDataSTD140& writeVec4(const float* data);
    UniformBufferRawDataSTD140& write(const glm::vec2& data);
    UniformBufferRawDataSTD140& write(const glm::vec3& data);
    UniformBufferRawDataSTD140& write(const glm::vec4& data);

    UniformBufferRawDataSTD140& writeDVec2(const double* data);
    UniformBufferRawDataSTD140& writeDVec3(const double* data);
    UniformBufferRawDataSTD140& writeDVec4(const double* data);
    UniformBufferRawDataSTD140& write(const glm::dvec2& data);
    UniformBufferRawDataSTD140& write(const glm::dvec3& data);
    UniformBufferRawDataSTD140& write(const glm::dvec4& data);

    template<typename T>
    UniformBufferRawDataSTD140& writeArray(const vector<T>& array){
        for (const T& value : array){
            m_write_offset = roundUpToMemoryBlock(m_write_offset, 16u);
            write(value);
        }
        return *this;
    }
};



class UniformBufferLayoutData : public UniformBufferData{
    MixedBufferLayout m_variables;
public:
    UniformBufferLayoutData();
    UniformBufferLayoutData(const MixedBufferLayout& variables);
    
    /**
     * Write data to variable with given index.
     * @param var_index index into the internal array of variables to write variable to.
     * @param val pointer to data_len variables of type T. T must be of type bool, int, uint, float or double.
     * @param data_len the number of values of given type to write.
     */
    template<typename T>
    UniformBufferLayoutData& write(uint32_t var_index, const T* val, uint32_t data_len){
        ShaderBasicType correct_type(*val);
        if (var_index >= m_variables.size())
        {
            PRINT_ERROR("Could not set mixed buffer data, variable index out of range.");
            return *this;
        }
        const BufferType& buffer_var = m_variables[var_index];
        if (buffer_var.type != correct_type)
        {
            PRINT_ERROR("Attempting to set uniform of incompatible type. Setting " << buffer_var.type.toString() << " with " << correct_type.toString() << " array.\n");
            return *this;
        }
        if (buffer_var.length != data_len)
        {
            PRINT_ERROR("Data to be set is not the same size as available space. Required length: " << buffer_var.length << ", Given data: " << data_len)
            return *this;
        }
        UniformBufferData::write(buffer_var.offset, val, m_variables[var_index].size());
        return *this;
    }

    /**
     * Write data to variable with given index.
     * @param var_index index into the internal array of variables to write variable to.
     * @param val vector to variables of type T. T must be of type bool, int, uint, float or double.
     */
    template<typename T>
    UniformBufferLayoutData& write(uint32_t var_index, const vector<T>& val){
        return write(var_index, val.data(), val.size());
    }

    /**
     * Write data to variable of given name.
     * @param name the name of the variable. For uniform buffers and push constants same as name in shaders.
     * @param val pointer to data_len variables of type T. T must be of type bool, int, uint, float or double.
     * @param data_len the number of values of given type to write.
     */
    template<typename T>
    UniformBufferLayoutData& write(const string& name, const T* val, uint32_t data_len){
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
    UniformBufferLayoutData& write(const string& name, const vector<T>& val){
        return write(name, val.data(), val.size());
    }

    /**
     * Write data to variable with given name
     * @param val the value to write
     */
    template<typename T>
    UniformBufferLayoutData& write(const string& name, T val){
        return write(name, &val, 1);
    }
};



#endif