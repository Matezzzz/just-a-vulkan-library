#include "parsing_utils.h"
#include "../01_device/allocator.h"
#include "shader_parser.h"
#include <fstream>





PipelineShaderStageInfo::PipelineShaderStageInfo(const string& compiled_shader_code, VkShaderStageFlagBits stage) :
    m_info{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0, stage, VK_NULL_HANDLE, "main", nullptr}
{
    //fill VkShaderModuleCreateInfo with data required for shader module creation
    VkShaderModuleCreateInfo info{
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,
        compiled_shader_code.size(), (uint32_t*) compiled_shader_code.data()
    };
    //create shader module - represents one compiled file
    m_info.module = g_allocator.get().createShaderModule(info);
}
PipelineShaderStageInfo::operator const VkPipelineShaderStageCreateInfo&() const{
    return m_info;
}




void findShaderChar(const char* str, uint32_t& offset, uint32_t len, char char_to_find){
    //inside how many curly braces is the loop
    int32_t depth = 0;
    //go through all characters of string until a match is found
    for (; offset < len; offset++){
        //current char
        char ch = str[offset];
        //if char is a match and isn't inside curly braces, return (char index is now in offset)
        if (depth == 0 && ch == char_to_find) return;
        //increase depth when { is found
        if (ch == '{') depth++;
        //try decreasing depth upon }, or print error and return, if depth is already zero.
        if (ch == '}') depth--;
    }
}


namespace parse{
    //used for all parsing functions of string and string_view, T is return type(string or string_view)
    template<typename T>
    T getUntilCharInternal(const char* data, uint32_t size, uint32_t& offset, char part_end_character, bool return_part_on_end){
        //save offset before searching
        uint32_t last_offset = offset;
        //find index of next occurence of char
        findShaderChar(data, offset, size, part_end_character);
        //if char wasn't found
        if (offset == size && !return_part_on_end) return T();
        //increase offset by 1 to point after found character
        //       pointer to first char, length
        return T(data + last_offset, offset++ - last_offset);
    }


    string string_view::getUntilChar(uint32_t& offset, char part_end_character, bool return_part_on_end) const{
        return getUntilCharInternal<string>(data(), size(), offset, part_end_character, return_part_on_end);
    }
    string_view string_view::getUntilCharView(uint32_t& offset, char part_end_character, bool return_part_on_end) const{
        return getUntilCharInternal<string_view>(data(), size(), offset, part_end_character, return_part_on_end);
    }
    string_view string_view::getSubstrView(uint32_t offset) const{
        return string_view(data() + offset, size() - offset);
    }
    string_view::operator string() const{
        return str();
    }
    string string_view::str() const{
        return string(data(), size());
    }


    string::string(const std::string& s) :
        std::string(s)
    {}
    string string::getUntilChar(uint32_t& offset, char part_end_character, bool return_part_on_end) const{
        return getUntilCharInternal<string>(data(), size(), offset, part_end_character, return_part_on_end);
    }
    string_view string::getUntilCharView(uint32_t& offset, char part_end_character, bool return_part_on_end) const{
        return getUntilCharInternal<string_view>(data(), size(), offset, part_end_character, return_part_on_end);
    }
    string_view string::getSubstrView(uint32_t offset) const{
        return string_view(data() + offset, size() - offset);
    }
    const string WHITESPACE(" \n\r\t\f\v");
    bool isWhitespace(char ch){
        return WHITESPACE.find(ch) != string::npos;
    }
    const string SPECIAL_CHARS = ",#()=;.{}[]";
    bool isSpecialChar(char ch){
        return SPECIAL_CHARS.find(ch) != string::npos;
    }
    parse::string& string::cleanRedundantWhitespaces(){
        //current index in resulting string
        uint32_t result_index = 0;
        //whether next space should be deleted
        bool delete_space = true;
        //whether char before this one was space
        bool last_was_space = false;
        //go over all string chars
        for (char ch : *this){
            if (isWhitespace(ch)){
                last_was_space = true;
            }else{
                //space is added if needed (only between 2 letters)
                if (last_was_space && !delete_space && !isSpecialChar(ch)) (*this)[result_index++] = ' ';
                //save current non-whitespace character and increase result index. This overwrites chars in current string, but max. one per each char already read
                (*this)[result_index++] = ch;

                last_was_space = false;
                //if current char isn't a letter, all spaces after it can be deleted without breaking syntax
                if (isSpecialChar(ch)) delete_space = true;
                else delete_space = false;
            }
        }
        //resize class to match amount of letters after removing whitespaces 
        this->resize(result_index);
        return *this;
    }
}


ShaderVariableType ShaderParseUtils::readShaderVariableType(parse::string_view name){
    //go through all possible shader variables, if names match, return ShaderVariableType
    for (uint32_t i = 0; i < SHADER_TYPE_COUNT; i++){
        ShaderVariableType a = static_cast<ShaderVariableType>(i);
        if (name == toString(a)) return a;
    }
    //return undefined if name wasn't found
    return SHADER_TYPE_UNDEFINED;
}
InputAttachmentDescriptorData* ShaderParseUtils::readInputAttachmentData(const DescriptorParameterVector& parameters){
    //create input attachment descriptor data with given input attachment index
    return new InputAttachmentDescriptorData{parameters.get("input_attachment_index")};
}
ShaderInOutData ShaderParseUtils::readShaderInOrOut(parse::string_view name_type, const DescriptorParameterVector& parameters){
    ShaderInOutData in_out_data;
    //read input location
    in_out_data.location = parameters.get("location");
    uint32_t offset = 0;
    //read type - first word, convert it to ShaderVariableType
    in_out_data.type = readShaderVariableType(name_type.getUntilCharView(offset, ' '));
    //remaining part of line is input/output name
    in_out_data.name = name_type.substr(offset);
    return in_out_data;
}
DescriptorQualifier ShaderParseUtils::readDescriptorQualifier(parse::string_view word)
{
    //go through all possible qualifiers, return DescriptorQualifier if names match
    for (uint32_t i = 0; i < DESCRIPTOR_QUALIFIER_NAMES.size(); i++){
        if (word == DESCRIPTOR_QUALIFIER_NAMES[i]) return static_cast<DescriptorQualifier>(i);
    }
    //return unrecognized if no match was found
    return QUALIFIER_UNRECOGNIZED;
}
SubsetVariableVector ShaderParseUtils::readSubsetVariablesData(parse::string_view type_info){
    //get index of first char inside curly braces
    uint32_t offset = type_info.find('{') + 1;
    //if no start was found, return empty subset variables vector
    if (offset == NPOS_32BIT) return {};
    //get curly braces end
    uint32_t end = type_info.find('}');
    
    SubsetVariableVector data;
    //currently parsed variable
    parse::string_view definition;
    //get variable while inside curly braces
    while ((definition = type_info.getUntilCharView(offset, ';')) != "" && offset <= end){
        uint32_t def_offset = 0;
        //read variable type(first word)
        ShaderVariableType type = readShaderVariableType(definition.getUntilCharView(def_offset, ' '));

        parse::string_view name = definition.getUntilCharView(def_offset, '[', true);
        //read descriptor count - move pointer back before [, then call read func
        uint32_t count = readDescriptorCount(definition.getSubstrView(def_offset - 1));
        SubsetVariable var(type, name, count);
        data.push_back(var);
    }
    return data;
}
uint32_t ShaderParseUtils::readDescriptorCount(parse::string_view name_word){
    //find opening brace
    uint32_t start = name_word.find('[');
    //if there is none, name_word isn't an array, return 1
    if (start == NPOS_32BIT) return 1;
    //find closing brace
    uint32_t end = name_word.find(']');
    //try converting number to int, print error if conversion fails
    try{
        return stoi(string(name_word.data() + start + 1, end - start - 1));
    }
    catch(std::invalid_argument& e){
        std::cerr << e.what() << '\n';
        DEBUG_ERROR("Failed to read descriptor count from string " << name_word.str())
        return 0;
    }
}
