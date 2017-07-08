#ifndef _src_renderer_vulkan_gpu_program_h_
#define _src_renderer_vulkan_gpu_program_h_

#include <yttrium/renderer/gpu_program.h>

namespace Yttrium
{
	class VulkanGpuProgram final : public GpuProgram
	{
	public:
		void set_uniform(const std::string&, const Matrix4&) override {}
	};
}

#endif
