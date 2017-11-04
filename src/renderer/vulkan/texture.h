#ifndef _src_renderer_vulkan_texture_h_
#define _src_renderer_vulkan_texture_h_

#include "../texture.h"
#include "handles.h"

namespace Yttrium
{
	class VulkanContext;

	class VulkanTexture2D final : public BackendTexture2D
	{
	public:
		VulkanTexture2D(RendererImpl&, VulkanContext&, const ImageFormat&, bool has_mipmaps, VkFormat, const void*);

	private:
		VK_HDeviceMemory _memory;
		const VK_HImage _image;
	};
}

#endif
