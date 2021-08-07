#ifndef SHADER_TYPES_H
#define SHADER_TYPES_H

#include "../00_base/vulkan_base.h"


 enum ShaderBasicTypesEnum
{
    SHADER_BOOL,
    SHADER_INT,
    SHADER_UINT,
    SHADER_FLOAT,
    SHADER_DOUBLE,
    SHADER_BASIC_TYPE_COUNT
};

class ShaderBasicType{
   
    ShaderBasicTypesEnum m_type;
    static constexpr uint32_t shader_basic_type_sizes[SHADER_BASIC_TYPE_COUNT]{1, 4, 4, 4, 8};
    static constexpr const char* shader_basic_type_names[SHADER_BASIC_TYPE_COUNT]{"bool", "int", "uint", "float", "double"};
public:
    ShaderBasicType(ShaderBasicTypesEnum type);
    ShaderBasicType(bool);
    ShaderBasicType(int32_t);
    ShaderBasicType(uint32_t);
    ShaderBasicType(float);
    ShaderBasicType(double);
    uint32_t sizeBytes() const;
    const char* toString() const;
    bool operator==(ShaderBasicType t) const;
    bool operator!=(ShaderBasicType t) const;
};



enum ShaderVariableType
{
    SHADER_TYPE_BOOL,   SHADER_TYPE_BVEC2, SHADER_TYPE_BVEC3, SHADER_TYPE_BVEC4,
    SHADER_TYPE_INT,    SHADER_TYPE_IVEC2, SHADER_TYPE_IVEC3, SHADER_TYPE_IVEC4,
    SHADER_TYPE_UINT,   SHADER_TYPE_UVEC2, SHADER_TYPE_UVEC3, SHADER_TYPE_UVEC4,
    SHADER_TYPE_FLOAT,  SHADER_TYPE_VEC2,  SHADER_TYPE_VEC3,  SHADER_TYPE_VEC4,
    SHADER_TYPE_DOUBLE, SHADER_TYPE_DVEC2, SHADER_TYPE_DVEC3, SHADER_TYPE_DVEC4,
    SHADER_TYPE_MAT_2,  SHADER_TYPE_MAT3,  SHADER_TYPE_MAT4,
    SHADER_TYPE_DMAT_2, SHADER_TYPE_DMAT3, SHADER_TYPE_DMAT4,
    SHADER_TYPE_UNDEFINED,
    SHADER_TYPE_COUNT
};


constexpr const char* shader_varible_type_names[]
{
    "bool", "bvec2", "bvec3", "bvec4", 
     "int", "ivec2", "ivec3", "ivec4",
    "uint", "uvec2", "uvec3", "uvec4",
   "float",  "vec2",  "vec3",  "vec4",
  "double", "dvec2", "dvec3", "dvec4",
    "mat2",  "mat3",  "mat4",
   "dmat2", "dmat3", "dmat4",
   "UNDEFINED"
};
constexpr const char* toString(ShaderVariableType type){
    return shader_varible_type_names[type];
}


constexpr uint32_t shader_variable_component_counts[]
{
    1, 2, 3, 4,
    1, 2, 3, 4,
    1, 2, 3, 4,
    1, 2, 3, 4,
    1, 2, 3, 4,
    4, 9, 16,
    4, 9, 16
};
constexpr uint32_t componentCount(ShaderVariableType type)
{
    return shader_variable_component_counts[type];
}


constexpr uint32_t shader_variable_uniform_alignments[]{
     1,  2,  4,  4,
     4,  8, 16, 16,
     4,  8, 16, 16,
     4,  8, 16, 16,
     8, 16, 32, 32,
     8, 16, 16,
    16, 32, 32
};
constexpr uint32_t STD430Alignment(ShaderVariableType type)
{
    return shader_variable_uniform_alignments[type];
}


const ShaderBasicType shader_variable_type_basic_types[]
{
      SHADER_BOOL,   SHADER_BOOL,   SHADER_BOOL,   SHADER_BOOL,
       SHADER_INT,    SHADER_INT,    SHADER_INT,    SHADER_INT,
      SHADER_UINT,   SHADER_UINT,   SHADER_UINT,   SHADER_UINT,
     SHADER_FLOAT,  SHADER_FLOAT,  SHADER_FLOAT,  SHADER_FLOAT,
    SHADER_DOUBLE, SHADER_DOUBLE, SHADER_DOUBLE, SHADER_DOUBLE,
     SHADER_FLOAT,  SHADER_FLOAT,  SHADER_FLOAT,
    SHADER_DOUBLE, SHADER_DOUBLE, SHADER_DOUBLE
};
ShaderBasicType basicType(ShaderVariableType type);
uint32_t sizeBytes(ShaderVariableType type);


#endif