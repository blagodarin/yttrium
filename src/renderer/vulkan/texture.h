#ifndef _src_renderer_vulkan_texture_h_
#define _src_renderer_vulkan_texture_h_

#include "../texture.h"

namespace Yttrium
{
	class VulkanTexture2D final : public BackendTexture2D
	{
	public:
		VulkanTexture2D(RendererImpl& renderer, const ImageFormat& format, bool has_mipmaps) : BackendTexture2D{renderer, format, has_mipmaps} {}
	};
}

#endif
