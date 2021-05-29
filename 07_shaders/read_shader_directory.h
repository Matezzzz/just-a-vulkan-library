#ifndef READ_SHADER_DIRECTORY_H
#define READ_SHADER_DIRECTORY_H

#include "../00_base/vulkan_base.h"


/**
 * ShaderData
 *  - Holds filename and data for both GLSL and compiled SPIRV shader parts. Holds shader stage identifier as well.
 */
class ShaderData{
public:
    string base_filename;
    string base_data;
    string compiled_filename;
    string compiled_data;
    VkShaderStageFlagBits shader_stage;
    //save stage, both filenames and read corresponding files into strings
    ShaderData(const string& dir, const string& base_filename, const string& compiled_filename, VkShaderStageFlagBits stage);
private:
    string readFileToString(const string& filename);
};

/**
 * ShaderDirectoryData
 *  - holds directory name and all shaders inside the directory
 */
class ShaderDirectoryData{
public:
    string dir_name;
    vector<ShaderData> shaders;
    ShaderDirectoryData(const string& shader_context_dir, const string& shader_dir);
};

/**
 * ShaderDirectoryTree
 *  - Holds all ShaderDirectoryDatas inside directory specified by path
 */
struct ShaderDirectoryTree{
public:
    string path;
    vector<ShaderDirectoryData> directories;
    ShaderDirectoryTree(const string& shader_context_dir);
};



#endif