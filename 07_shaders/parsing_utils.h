#ifndef PARSING_UTILS_H
#define PARSING_UTILS_H

#include "../00_base/vulkan_base.h"
#include "descriptor_types.h"
#include <string>
#include <string_view>
#include "shader_types.h"


using std::ifstream;




/**
 * PipelineShaderStageInfo
 *  - Represents info required to create one shader stage
 */
class PipelineShaderStageInfo{
    VkPipelineShaderStageCreateInfo m_info;
public:
    /**
     * Create shader create info from given SPIR-V code.
     * @param compiled_spirv_shader_code read from .spv file. Is converted to vulkan object by constructor, reference can stop being valid after call.
     * @param stage the shader stage this code will be used for
     */
    PipelineShaderStageInfo(const string& compiled_spirv_shader_code, VkShaderStageFlagBits stage);
    operator const VkPipelineShaderStageCreateInfo&() const;
};


#define NPOS_32BIT ~0U



enum DescriptorQualifier{
    QUALIFIER_WRITE_ONLY,
    QUALIFIER_READ_ONLY,
    QUALIFIER_RESTRICT,
    QUALIFIER_COHERENT,
    QUALIFIER_UNRECOGNIZED
};
const vector<string> DESCRIPTOR_QUALIFIER_NAMES{"writeonly", "readonly", "restrict", "coherent"};


/**
 * Find next occurence of char in char array str. All characters inside curly braces {} are ignored.
 * @param pointer to start of string to search in
 * @param offset offset to start from, it is modified to point to the found char
 * @param len length of the string to search. Exit after offset gets to len.
 * @param char_to_find
 */
void findShaderChar(const char* str, uint32_t& offset, uint32_t len, char char_to_find);



//a namespace for string and string_view with parsing functions
namespace parse{
    class string;
    /**
     * NiceStringView
     *  - std::string_view with some additional parsing functions
     */
    class string_view : public std::string_view{
    public:
        using std::string_view::string_view;
        
        /**
         * Get string until next occurence of character. Characters inside curly braces {} are skipped.
         * @param offset the offset to start at, and modify to match index of ending character
         * @param part_end_character the character to end at
         * @param return_part_on_end whether to return remaining part of string if character isn't found
         */
        string getUntilChar(uint32_t& offset, char part_end_character, bool return_part_on_end = false) const;
        
        /**
         * Get string view until next occurence of character. Characters inside curly braces {} are skipped.
         * @param offset the offset to start at, and modify to match index of ending character
         * @param part_end_character the character to end at
         * @param return_part_on_end whether to return remaining part of string if character isn't found
         */
        string_view getUntilCharView(uint32_t& offset, char part_end_character, bool return_part_on_end = false) const;
        
        /**
         * Return part of string view from offset to end
         * @param offset the offset
         */
        string_view getSubstrView(uint32_t offset) const;
        operator string() const;
        string str() const;
    };

    /**
     * NiceString
     *  - std::string with some additional parsing functions
     */
    class string : public std::string{
    public:
        using std::string::string;
        string(const std::string& s);

        /**
         * Get string until next occurence of character. Characters inside curly braces {} are skipped.
         * @param offset the offset to start at, and modify to match index of ending character
         * @param part_end_character the character to end at
         * @param return_part_on_end whether to return remaining part of string if character isn't found
         */
        string getUntilChar(uint32_t& offset, char part_end_character, bool return_part_on_end = false) const;

        /**
         * Get string view until next occurence of character. Characters inside curly braces {} are skipped.
         * @param offset the offset to start at, and modify to match index of ending character
         * @param part_end_character the character to end at
         * @param return_part_on_end whether to return remaining part of string if character isn't found
         */
        string_view getUntilCharView(uint32_t& offset, char part_end_character, bool return_part_on_end = false) const;

        /**
         * Return string view from offset to end
         * @param offset the offset
         */
        string_view getSubstrView(uint32_t offset) const;

        //remove all double whitespaces and those before/after braces and few other special symbols
        string& cleanRedundantWhitespaces();
    };
}



class InputAttachmentDescriptorData;
class SubsetVariableVector;
class ShaderInOutData;
class DescriptorParameterVector;


/**
 * ShaderParseUtils
 *  - Class holding multiple static functions for parsing shaders
 */
class ShaderParseUtils{
public:
    //read input attachment data from vector of descriptor parameters
    static InputAttachmentDescriptorData* readInputAttachmentData(const DescriptorParameterVector& params);

    //read subset variables from given string
    static SubsetVariableVector readSubsetVariablesData(parse::string_view type_word);

    /**
     * Read shader input / output. These are vertex buffer entries as input and fragment shader color as output for a normal pipeline.
     * @param name_type string containing name and type of the input. E.g. 'vec2 v_position'
     * @param parameters all parameters connected to this input/output
     */
    static ShaderInOutData readShaderInOrOut(parse::string_view name_type, const DescriptorParameterVector& parameters);

    /**
     * Read qualifier from given string.
     * @param word E.g. 'writeonly'
     */
    static DescriptorQualifier readDescriptorQualifier(parse::string_view word);

    /**
     * Read descriptor count. Returns one in case descriptor isn't an array
     * @param name_word name, possibly with array size 'textures[5]'
     */
    static uint32_t readDescriptorCount(parse::string_view name_word);

    /**
     * Read shader variable type from the string view.
     * @param view E.g. 'mat4'
     */
    static ShaderVariableType readShaderVariableType(parse::string_view v);
};

#endif