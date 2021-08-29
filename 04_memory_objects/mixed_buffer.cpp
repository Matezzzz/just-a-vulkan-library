#include "mixed_buffer.h"








BufferLayoutCreateType::BufferLayoutCreateType(const string& n, ShaderVariableType t, uint32_t a_l, uint32_t o) :
    name(n), offset(o), base_length(componentCount(t)), array_length(a_l), type(basicType(t)), required_memory_multiple(STD430Alignment(t))
{}
uint32_t BufferLayoutCreateType::elementCount() const{
    return array_length * base_length;
}




BufferType::BufferType(uint32_t _offset, uint32_t _length, ShaderBasicType _type, const string& _name) :
    offset(_offset), length(_length), type(_type), name(_name)
{}

uint32_t BufferType::size() const{
    return length;
}
uint32_t BufferType::sizeBytes() const{
    return type.sizeBytes() * length;
}
uint32_t BufferType::end() const{
    return offset + sizeBytes();
}
bool BufferType::operator==(const BufferType& t2) const{
    return (offset == t2.offset && length == t2.length && type == t2.type && name == t2.name);
}
bool BufferType::overlaps(const BufferType& t2) const{
    uint32_t e1 = end();
    uint32_t e2 = t2.end();
    //types don't overlap if end is before other's start or other's start is after start
    return !(e1 <= t2.offset || offset >= e2);
}







MixedBufferLayout::MixedBufferLayout()
{}
MixedBufferLayout::MixedBufferLayout(const vector<BufferType>& variables) : vector<BufferType>{variables}
{}
uint32_t MixedBufferLayout::find(const string& name){
    //for all variables: if name is the one being looked for, return index ofa variable
    for (uint32_t i = 0; i < size(); i++){
        if ((*this)[i].name == name) return i;
    }
    //print error if no variable of given name was found
    PRINT_ERROR("Variable of name '" << name << "' couldn't be found.")
    return (~0U);
}
uint32_t MixedBufferLayout::getSize() const{
    return m_size;
}
void MixedBufferLayout::setSize(uint32_t size){
    m_size = size;
}




MixedBufferLayout BufferLayoutCreateTypeVector::create(uint32_t first_var_offset){
    MixedBufferLayout layout;
    uint32_t cur_offset = first_var_offset;
    //go through all mixed buffer variables
    for (auto var : (*this)){
        //if the offset is not specified, compute it by rounding up current offset to required memory multiple
        if (var.offset == OFFSET_NOT_SPECIFIED){
            cur_offset = roundUpToMemoryBlock(cur_offset, var.required_memory_multiple);
        }else{
            cur_offset = var.offset;
        }
        //create buffer type with offset computed beforehand, type and name
        layout.push_back(BufferType{cur_offset, var.elementCount(), var.type, var.name});
        //add current element size in bytes of to cur_offset
        cur_offset += var.type.sizeBytes() * var.base_length * var.array_length;
    }
    //cur_offset now contains size in bytes of all subset variables, set it as 
    layout.setSize(cur_offset);
    return layout;
}


void UniformBufferRawData::resetOffset(){
    m_write_offset = 0;
}

vector<uint32_t> convertArrayOfBools(uint32_t len, const bool* data){
    vector<uint32_t> v;
    v.reserve(len);
    for (uint32_t i = 0; i < len; i++) v.push_back(data[i]);
    return v;
}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(bool     n){return write<bool    , 1, 4>(&n);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(int32_t  n){return write<int32_t , 1, 4>(&n);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(uint32_t n){return write<uint32_t, 1, 4>(&n);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(float    n){return write<float   , 1, 4>(&n);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(double   n){return write<double  , 1, 8>(&n);}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeBVec2(const bool* data){return write<uint32_t, 2,  8>(convertArrayOfBools(2, data).data());}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeBVec3(const bool* data){return write<uint32_t, 3, 16>(convertArrayOfBools(2, data).data());}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeBVec4(const bool* data){return write<uint32_t, 4, 16>(convertArrayOfBools(2, data).data());}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::bvec2& data){return writeBVec2(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::bvec3& data){return writeBVec3(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::bvec4& data){return writeBVec4(glm::value_ptr(data));}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeIVec2(const int32_t* data){return write<int32_t, 2,  8>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeIVec3(const int32_t* data){return write<int32_t, 3, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeIVec4(const int32_t* data){return write<int32_t, 4, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::ivec2& data){return writeIVec2(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::ivec3& data){return writeIVec3(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::ivec4& data){return writeIVec4(glm::value_ptr(data));}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeUVec2(const uint32_t* data){return write<uint32_t, 2,  8>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeUVec3(const uint32_t* data){return write<uint32_t, 3, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeUVec4(const uint32_t* data){return write<uint32_t, 4, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::uvec2& data){return writeUVec2(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::uvec3& data){return writeUVec3(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::uvec4& data){return writeUVec4(glm::value_ptr(data));}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeVec2(const float* data){return write<float, 2,  8>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeVec3(const float* data){return write<float, 3, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeVec4(const float* data){return write<float, 4, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::vec2& data){return writeVec2(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::vec3& data){return writeVec3(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::vec4& data){return writeVec4(glm::value_ptr(data));}

UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeDVec2(const double* data){return write<double, 2, 16>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeDVec3(const double* data){return write<double, 3, 32>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::writeDVec4(const double* data){return write<double, 4, 32>(data);}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::dvec2& data){return writeDVec2(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::dvec3& data){return writeDVec3(glm::value_ptr(data));}
UniformBufferRawDataSTD140& UniformBufferRawDataSTD140::write(const glm::dvec4& data){return writeDVec4(glm::value_ptr(data));}




UniformBufferLayoutData::UniformBufferLayoutData() : m_variables{{}}
{}
UniformBufferLayoutData::UniformBufferLayoutData(const MixedBufferLayout& variables) : UniformBufferData(variables.getSize()), m_variables(variables)
{}