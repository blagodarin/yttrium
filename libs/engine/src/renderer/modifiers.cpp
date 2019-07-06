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

#include <yttrium/renderer/modifiers.h>

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/program.h>
#include <yttrium/renderer/texture.h>
#include "material.h"
#include "pass.h"

namespace Yttrium
{
	Push2D::Push2D(RenderPass& pass)
		: RenderModifier{ pass }
	{
		static_cast<RenderPassImpl&>(_pass).push_projection_2d(Matrix4::projection_2d(pass.window_size()));
	}

	Push2D::~Push2D() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_projection();
	}

	Push3D::Push3D(RenderPass& pass, const Matrix4& projection, const Matrix4& view)
		: RenderModifier{ pass }
	{
		static_cast<RenderPassImpl&>(_pass).push_projection_3d(projection, view);
	}

	Push3D::~Push3D() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_projection();
	}

	PushProgram::PushProgram(RenderPass& pass, const RenderProgram* program)
		: RenderModifier{ pass }
	{
		static_cast<RenderPassImpl&>(_pass).push_program(program);
	}

	PushProgram::~PushProgram() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_program();
	}

	PushMaterial::PushMaterial(RenderPass& pass, const Material* material)
		: RenderModifier{ pass }
		, _material{ material }
		, _program{ pass, &static_cast<const MaterialImpl*>(_material)->program() }
		, _texture{ pass, static_cast<const MaterialImpl*>(_material)->texture(), static_cast<const MaterialImpl*>(_material)->texture_filter() }
	{
	}

	void PushMaterial::set_uniform(const std::string& name, const Matrix4& value)
	{
		const_cast<MaterialImpl*>(static_cast<const MaterialImpl*>(_material))->program().set_uniform(name, value); // TODO: Remove 'const_cast'.
	}

	PushTexture::PushTexture(RenderPass& pass, const Texture2D* texture, Flags<Texture2D::Filter> filter)
		: RenderModifier{ pass }
		, _filter{ static_cast<RenderPassImpl&>(_pass).push_texture(texture, filter) }
	{
	}

	PushTexture::~PushTexture() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_texture(_filter);
	}

	PushTransformation::PushTransformation(RenderPass& pass, const Matrix4& matrix)
		: RenderModifier{ pass }
	{
		static_cast<RenderPassImpl&>(_pass).push_transformation(matrix);
	}

	PushTransformation::~PushTransformation() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_transformation();
	}
}
