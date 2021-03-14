// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

namespace Yt
{
	class RenderBackend;
	class RenderProgram;
	class Texture2D;

	class RenderBuiltin
	{
	public:
		const std::shared_ptr<const Texture2D> _white_texture;
		const std::unique_ptr<RenderProgram> _program_2d;

		explicit RenderBuiltin(RenderBackend&);
		~RenderBuiltin() noexcept;
	};
}
