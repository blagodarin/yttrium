#ifndef _src_renderer_vulkan_gpu_program_h_
#define _src_renderer_vulkan_gpu_program_h_

#include <yttrium/renderer/gpu_program.h>

namespace Yttrium
{
	class VulkanRenderer;

	class VulkanGpuProgram final : public GpuProgram
	{
	public:
		explicit VulkanGpuProgram(VulkanRenderer& renderer) : _renderer{renderer} {}

		void set_uniform(const std::string&, const Matrix4&) override;

	private:
		VulkanRenderer& _renderer;
	};
}

#endif
