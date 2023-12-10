// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/base/flags.h>
#include <yttrium/renderer/texture.h>

#include <string>

namespace seir
{
	class Mat4;
}

namespace Yt
{
	class Material;
	class RenderPass;
	class RenderProgram;
	class SizeF;

	/// Base class for RenderPass modifiers.
	class RenderModifier
	{
	public:
		RenderModifier(const RenderModifier&) = delete;
		RenderModifier& operator=(const RenderModifier&) = delete;

	protected:
		RenderPass& _pass;
		explicit RenderModifier(RenderPass& pass) noexcept
			: _pass{ pass } {}
	};

	///
	class Push3D : public RenderModifier
	{
	public:
		///
		Push3D(RenderPass&, const seir::Mat4& projection, const seir::Mat4& view);

		///
		~Push3D() noexcept;
	};

	///
	class PushProgram : public RenderModifier
	{
	public:
		///
		PushProgram(RenderPass&, const RenderProgram*);

		///
		~PushProgram() noexcept;
	};

	///
	class PushTexture : public RenderModifier
	{
	public:
		///
		PushTexture(RenderPass&, const Texture2D*, Flags<Texture2D::Filter> = Texture2D::NearestFilter);

		///
		~PushTexture() noexcept;

	private:
		const Flags<Texture2D::Filter> _filter;
	};

	///
	class PushMaterial : public RenderModifier
	{
	public:
		///
		PushMaterial(RenderPass&, const Material*);

		///
		void set_uniform(const std::string&, const seir::Mat4&);

	private:
		const Material* const _material;
		PushProgram _program;
		PushTexture _texture;
	};

	///
	class PushTransformation : public RenderModifier
	{
	public:
		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(RenderPass&, const seir::Mat4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation() noexcept;
	};
}
