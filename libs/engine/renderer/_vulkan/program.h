#ifndef _src_renderer_vulkan_program_h_
#define _src_renderer_vulkan_program_h_

#include <yttrium/renderer/program.h>

namespace Yttrium
{
	class VulkanRenderer;

	class VulkanProgram final : public RenderProgram
	{
	public:
		explicit VulkanProgram(VulkanRenderer& renderer)
			: _renderer{ renderer } {}

		void set_uniform(const std::string&, const Matrix4&) override;

	private:
		VulkanRenderer& _renderer;
	};
}

#endif
