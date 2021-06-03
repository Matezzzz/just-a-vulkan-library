//This includes all vulkan library files

#include "00_base/vulkan_base.h"
#include "00_base/extension_utilities.h"
#include "00_base/vulkan_enum_strings.h"

#include "01_device/vulkan_instance.h"
#include "01_device/allocator.h"
#include "01_device/physical_device.h"
#include "01_device/device.h"

#include "02_swapchain/swapchain.h"
#include "02_swapchain/swapchain_info.h"
#include "02_swapchain/window.h"

#include "03_commands/command_pool.h"
#include "03_commands/synchronization.h"
#include "03_commands/command_buffer.h"

#include "04_memory_objects/buffer.h"
#include "04_memory_objects/buffer_info.h"
#include "04_memory_objects/image.h"
#include "04_memory_objects/image_info.h"
#include "04_memory_objects/local_object_creator.h"
#include "04_memory_objects/mixed_buffer.h"

#include "05_descriptor_sets/sampler.h"
#include "05_descriptor_sets/descriptor_pool.h"

#include "06_render_passes/framebuffer.h"
#include "06_render_passes/renderpass_specializations.h"
#include "06_render_passes/renderpass.h"

#include "07_shaders/descriptor_data.h"
#include "07_shaders/descriptor_types.h"
#include "07_shaders/parsing_utils.h"
#include "07_shaders/pipelines_context.h"
#include "07_shaders/read_shader_directory.h"
#include "07_shaders/shader_parser.h"
#include "07_shaders/shader_types.h"

#include "08_pipeline/blend_info.h"
#include "08_pipeline/depth_stencil_info.h"
#include "08_pipeline/multisample_info.h"
#include "08_pipeline/pipeline.h"
#include "08_pipeline/rasterization_info.h"
#include "08_pipeline/simple_infos.h"
#include "08_pipeline/viewport_scissor_info.h"

#include "09_utilities/camera.h"
#include "09_utilities/data_creator.h"
#include "09_utilities/image_load.h"

