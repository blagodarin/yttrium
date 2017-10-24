#ifndef _src_renderer_vulkan_glsl_h_
#define _src_renderer_vulkan_glsl_h_

#include <vector>

#include <vulkan/vulkan.h>

namespace Yttrium
{
	std::vector<uint32_t> glsl_to_spirv(const char*, VkShaderStageFlagBits);
}

#endif
