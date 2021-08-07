#include "shader_types.h"



ShaderBasicType::ShaderBasicType(ShaderBasicTypesEnum type) : m_type(type){}
ShaderBasicType::ShaderBasicType(bool) : m_type(SHADER_BOOL){}
ShaderBasicType::ShaderBasicType(int32_t) : m_type(SHADER_INT){}
ShaderBasicType::ShaderBasicType(uint32_t) : m_type(SHADER_UINT){}
ShaderBasicType::ShaderBasicType(float) : m_type(SHADER_FLOAT){}
ShaderBasicType::ShaderBasicType(double) : m_type(SHADER_DOUBLE){}
uint32_t ShaderBasicType::sizeBytes() const{
    return shader_basic_type_sizes[(int) m_type];
}
const char* ShaderBasicType::toString() const{
    return shader_basic_type_names[(int) m_type];
}
bool ShaderBasicType::operator==(ShaderBasicType t) const{
    return (m_type == t.m_type);
}
bool ShaderBasicType::operator!=(ShaderBasicType t) const{
    return !((*this) == t);
}





ShaderBasicType basicType(ShaderVariableType type)
{
    return shader_variable_type_basic_types[type];
}
uint32_t sizeBytes(ShaderVariableType type)
{
    return basicType(type).sizeBytes() * componentCount(type);
}
