//
// Copyright 2019 Sergei Blagodarin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef _src_renderer_vulkan_texture_h_
#define _src_renderer_vulkan_texture_h_

#include "../texture.h"
#include "handles.h"

namespace Yttrium
{
	class VulkanTexture2D final : public BackendTexture2D
	{
	public:
		VulkanTexture2D(RenderBackend&, VulkanContext&, const ImageInfo&, bool has_mipmaps, VkFormat, const void*);

		VkDescriptorImageInfo descriptor_image_info() const noexcept;

	private:
		VK_HDeviceMemory _memory;
		const VK_HImage _image;
		VK_HImageView _view;
		VK_HSampler _sampler;
	};
}

#endif
