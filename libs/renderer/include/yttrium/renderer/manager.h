// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/flags.h>

#include <memory>
#include <string>

namespace seir
{
	class Blob;
	class Image;
}

namespace Yt
{
	class Mesh;
	class RenderProgram;
	class Texture2D;

	///
	/// \note Lifetimes of entities created by a RenderManager must not exceed the lifetime of the RenderManager.
	class RenderManager
	{
	public:
		///
		enum class TextureFlag
		{
			NoMipmaps = 1 << 0,
		};

		///
		virtual ~RenderManager() noexcept = default;

		///
		virtual std::unique_ptr<RenderProgram> create_program(const std::string& vertex_shader, const std::string& fragment_shader) = 0;

		///
		virtual std::unique_ptr<Texture2D> create_texture_2d(const seir::Image&, Flags<TextureFlag> = {}) = 0;

		///
		virtual std::unique_ptr<Mesh> load_mesh(const seir::Blob&, std::string_view source_name) = 0;
	};
}
