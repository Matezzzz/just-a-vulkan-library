#include "vulkan_enum_strings.h"


#define str(x) std::to_string((int) x)


string indexedToString(uint32_t i, string arr[], uint32_t arr_bytes_len){
    if (i >= arr_bytes_len / sizeof(string)) return str(i);
    return arr[i];
}

string flagsToString(uint32_t f, string arr[], uint32_t arr_bytes_len){
    if (f == 0) return "FLAGS_NONE";
    string result = "";
    bool first = true;
    for (uint32_t i = 0; i < arr_bytes_len / sizeof(string); i++){
        if ((1 << i) & f){
            if (!first) result += "|";
            result += arr[i];
            first = false;
        }
    }
    return result;
}



#ifdef DEBUG



string VkImageLayoutStrings[] {"VK_IMAGE_LAYOUT_UNDEFINED", "VK_IMAGE_LAYOUT_GENERAL", "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL", "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL", "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL", "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL", "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL", "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL", "VK_IMAGE_LAYOUT_PREINITIALIZED"};
string VkImageLayoutToString(VkImageLayout l){
    return indexedToString(l, VkImageLayoutStrings, sizeof(VkImageLayoutStrings));
}
string VkImageAccessStrings[]{"VK_ACCESS_INDIRECT_COMMAND_READ_BIT", "VK_ACCESS_INDEX_READ_BIT", "VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT", "VK_ACCESS_UNIFORM_READ_BIT", "VK_ACCESS_INPUT_ATTACHMENT_READ_BIT", "VK_ACCESS_SHADER_READ_BIT", "VK_ACCESS_SHADER_WRITE_BIT", "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT", "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT", "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT", "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT", "VK_ACCESS_TRANSFER_READ_BIT", "VK_ACCESS_TRANSFER_WRITE_BIT", "VK_ACCESS_HOST_READ_BIT", "VK_ACCESS_HOST_WRITE_BIT", "VK_ACCESS_MEMORY_READ_BIT", "VK_ACCESS_MEMORY_WRITE_BIT", "VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT", "VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT", "VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT", "VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT", "VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT", "VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR", "VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR", "VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV", "VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT", "VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV", "VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV"};
string VkAccessFlagsToString(VkAccessFlags a){
    return flagsToString(a, VkImageAccessStrings, sizeof(VkImageAccessStrings));
}

#else

string VkImageLayoutToString(VkImageLayout l){
    return str(l);
}
string VkAccessToString(VkAccessFlags a){
    return str(a);
}

#endif

#undef str