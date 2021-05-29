#include "mixed_buffer.h"


uint64_t roundUpToMemoryBlock(uint64_t size, uint64_t block_size){
    //compute value surely in the next block
    uint64_t a = size + block_size - 1;
    //round value to the start of block - start of next block has to be end of current one
    return roundDownToMemoryBlock(a, block_size);
}
uint64_t roundDownToMemoryBlock(uint64_t size, uint64_t block_size){
    //(size % block_size) computes offset from current block start
    //subtract offset from size and return
    return size - (size % block_size);
}






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
    return sizeBytes(type) * length;
}
uint32_t BufferType::end() const{
    return offset + size();
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
        cur_offset += sizeBytes(var.type) * var.base_length * var.array_length;
    }
    //cur_offset now contains size in bytes of all subset variables, set it as 
    layout.setSize(cur_offset);
    return layout;
}






MixedBufferData::MixedBufferData() : m_variables{{}}
{}
MixedBufferData::MixedBufferData(const MixedBufferLayout& variables) : vector<uint8_t>(variables.getSize(), 0), m_variables(variables)
{}