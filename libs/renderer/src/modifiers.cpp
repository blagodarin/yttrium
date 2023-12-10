// This file is part of the Yttrium toolkit.
// Copyright (C) Sergei Blagodarin.
// SPDX-License-Identifier: Apache-2.0

#include <yttrium/renderer/modifiers.h>

#include <yttrium/renderer/program.h>
#include <yttrium/renderer/texture.h>
#include "material.h"
#include "pass.h"

namespace Yt
{
	Push3D::Push3D(RenderPass& pass, const seir::Mat4& projection, const seir::Mat4& view)
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

	void PushMaterial::set_uniform(const std::string& name, const seir::Mat4& value)
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

	PushTransformation::PushTransformation(RenderPass& pass, const seir::Mat4& matrix)
		: RenderModifier{ pass }
	{
		static_cast<RenderPassImpl&>(_pass).push_transformation(matrix);
	}

	PushTransformation::~PushTransformation() noexcept
	{
		static_cast<RenderPassImpl&>(_pass).pop_transformation();
	}
}
