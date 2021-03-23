// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/program.h>

namespace Yt
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
