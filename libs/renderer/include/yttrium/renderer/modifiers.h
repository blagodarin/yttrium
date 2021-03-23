// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <yttrium/renderer/texture.h>
#include <yttrium/utils/flags.h>

#include <string>

namespace Yt
{
	class Material;
	class Matrix4;
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
		Push3D(RenderPass&, const Matrix4& projection, const Matrix4& view);

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
		void set_uniform(const std::string&, const Matrix4&);

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
		PushTransformation(RenderPass&, const Matrix4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation() noexcept;
	};
}
