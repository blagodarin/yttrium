// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../../model/mesh_data.h"

#include <vulkan/vulkan.h>

namespace Yt
{
	class VulkanVertexFormat
	{
	public:
		std::vector<VkVertexInputAttributeDescription> _attributes;
		VkVertexInputBindingDescription _binding;
		VkPipelineVertexInputStateCreateInfo _input;
		VkPipelineInputAssemblyStateCreateInfo _assembly;

		explicit VulkanVertexFormat(const std::vector<VA>&);

		VulkanVertexFormat(const VulkanVertexFormat&) = delete;
		VulkanVertexFormat& operator=(const VulkanVertexFormat&) = delete;
	};
}
