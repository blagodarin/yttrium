// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/material.h>
#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yt
{
	class RenderProgram;

	class MaterialImpl final : public Material
	{
	public:
		MaterialImpl(std::unique_ptr<RenderProgram>&&, std::shared_ptr<const Texture2D>&&, Texture2D::Filter);

		RenderProgram& program() noexcept { return *_program; }
		const RenderProgram& program() const noexcept { return *_program; }
		const Texture2D* texture() const noexcept { return _texture.get(); }
		Texture2D::Filter texture_filter() const noexcept { return _texture_filter; }

	protected:
		const std::unique_ptr<RenderProgram> _program;
		const std::shared_ptr<const Texture2D> _texture;
		const Texture2D::Filter _texture_filter;
	};
}
