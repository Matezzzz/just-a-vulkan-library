#include "read_shader_directory.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


//all shader stages
enum ShaderStage{VERTEX, GEOMETRY, TESSELATION_EVALUATION, TESSELATION_CONTROL, FRAGMENT, COMPUTE, SHADER_STAGE_COUNT, SHADER_STAGE_INVALID};
//corresponding vulkan flags
const VkShaderStageFlagBits vulkan_shader_stages[]{VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_GEOMETRY_BIT, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, VK_SHADER_STAGE_FRAGMENT_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};
VkShaderStageFlagBits getVulkanShaderStage(ShaderStage s) {return vulkan_shader_stages[s];}
//corresponding file extensions
const string shader_stage_extensions[]{".vert", ".geom", ".tese", ".tesc", ".frag", ".comp"};
ShaderStage getShaderStage(const string& stage_ext){
    //go through all shader stages, if extension is same, return ShaderStage
    for (uint32_t i = 0; i < SHADER_STAGE_COUNT; i++){
        if (shader_stage_extensions[i] == stage_ext) return (ShaderStage) i;
    }
    return SHADER_STAGE_INVALID;
}



ShaderData::ShaderData(const string& dir, const string& base_filename_, const string& compiled_filename_, VkShaderStageFlagBits stage) :
    base_filename(base_filename_), base_data(readFileToString(dir + "/" + base_filename)),
    compiled_filename(compiled_filename_), compiled_data(readFileToString(dir + "/" + compiled_filename)),
    shader_stage(stage)
{}
string ShaderData::readFileToString(const string& filename){
    //open file in binary mode
    std::ifstream file(filename, std::ios::binary);
    //read contents to string
    string file_data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    //if string has length 0 if file couldn't be found, print error
    if (file_data.size() == 0) PRINT_ERROR("File \'" << filename << "\' not found");
    return file_data;
}



ShaderDirectoryData::ShaderDirectoryData(const string& shader_context_dir, const string& shader_dir) : dir_name(shader_dir){
    //go through all files in current directory
    for (auto& f : fs::directory_iterator(shader_context_dir + "/" + dir_name)){
        //skip all directories
        if (f.is_directory()) continue;

        //get shader extension from filename
        string shader_ext = f.path().extension().string();
        //if it's .spv, compiled shader file, skip it
        if (shader_ext == ".spv") continue;
        //deduce shader stage from extension
        ShaderStage stage = getShaderStage(shader_ext);
        //if extension couldn't be identified, print error and skip this file
        if (stage == SHADER_STAGE_INVALID){
            PRINT_ERROR("Invalid shader suffix:" << shader_ext)
            continue;
        }
        //if extension is legit, add file to the list of current shaders
        shaders.push_back(
            ShaderData{
                shader_context_dir + "/" + dir_name,
                f.path().filename().string(),
                shader_ext.substr(1) + ".spv",
                getVulkanShaderStage(stage)
            }
        );
    }
}



ShaderDirectoryTree::ShaderDirectoryTree(const string& shader_context_dir) : path(shader_context_dir){
    //go through all files/dirs in given path
    for (auto& f : fs::directory_iterator(path)){
        //if current object is a directory, create ShaderDirectoryData from it
        if (f.is_directory()){
            directories.push_back(ShaderDirectoryData(shader_context_dir, f.path().filename().string()));
        }
    }
}