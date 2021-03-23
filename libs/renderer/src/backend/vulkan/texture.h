// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../texture.h"
#include "handles.h"

namespace Yt
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
