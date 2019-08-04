//
// This file is part of the Yttrium toolkit.
// Copyright (C) 2019 Sergei Blagodarin.
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

#pragma once

#include <yttrium/api.h>
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
	class Y_ENGINE_API RenderModifier
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
	class Y_ENGINE_API Push2D : public RenderModifier
	{
	public:
		///
		explicit Push2D(RenderPass&);

		///
		~Push2D() noexcept;
	};

	///
	class Y_ENGINE_API Push3D : public RenderModifier
	{
	public:
		///
		Push3D(RenderPass&, const Matrix4& projection, const Matrix4& view);

		///
		~Push3D() noexcept;
	};

	///
	class Y_ENGINE_API PushProgram : public RenderModifier
	{
	public:
		///
		PushProgram(RenderPass&, const RenderProgram*);

		///
		~PushProgram() noexcept;
	};

	///
	class Y_ENGINE_API PushTexture : public RenderModifier
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
	class Y_ENGINE_API PushMaterial : public RenderModifier
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
	class Y_ENGINE_API PushTransformation : public RenderModifier
	{
	public:
		/// Multiplies the current transformation matrix by the specified one,
		/// pushes it to the transformation stack and applies the resulting transformation.
		PushTransformation(RenderPass&, const Matrix4&);

		/// Pops a matrix from the transformation stack and applies the previous matrix.
		~PushTransformation() noexcept;
	};
}
