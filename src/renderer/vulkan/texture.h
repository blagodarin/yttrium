#ifndef _src_renderer_vulkan_texture_h_
#define _src_renderer_vulkan_texture_h_

#include "../texture.h"
#include "handles.h"

namespace Yttrium
{
	class VulkanTexture2D final : public BackendTexture2D
	{
	public:
		VulkanTexture2D(RendererImpl&, VulkanContext&, const ImageFormat&, bool has_mipmaps, VkFormat, const void*);

		VkDescriptorImageInfo descriptor_image_info() const noexcept;

	private:
		VK_HDeviceMemory _memory;
		const VK_HImage _image;
		VK_HImageView _view;
		VK_HSampler _sampler;
	};
}

#endif
