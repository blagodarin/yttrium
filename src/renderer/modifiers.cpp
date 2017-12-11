#include <yttrium/renderer/modifiers.h>

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/texture.h>
#include "context.h"
#include "material.h"

namespace Yttrium
{
	Push2D::Push2D(RenderContext& context)
		: RenderModifier{context}
	{
		static_cast<RenderContextImpl&>(_context).push_projection_2d(Matrix4::projection_2d(context.window_size()));
	}

	Push2D::~Push2D() noexcept
	{
		static_cast<RenderContextImpl&>(_context).pop_projection();
	}

	Push3D::Push3D(RenderContext& context, const Matrix4& projection, const Matrix4& view)
		: RenderModifier{context}
	{
		static_cast<RenderContextImpl&>(_context).push_projection_3d(projection, view);
	}

	Push3D::~Push3D() noexcept
	{
		static_cast<RenderContextImpl&>(_context).pop_projection();
	}

	PushGpuProgram::PushGpuProgram(RenderContext& context, const GpuProgram* program)
		: RenderModifier{context}
	{
		static_cast<RenderContextImpl&>(_context).push_program(program);
	}

	PushGpuProgram::~PushGpuProgram() noexcept
	{
		static_cast<RenderContextImpl&>(_context).pop_program();
	}

	PushMaterial::PushMaterial(RenderContext& context, const Material* material)
		: RenderModifier{context}
		, _material{material}
		, _gpu_program{context, &static_cast<const MaterialImpl*>(_material)->gpu_program()}
		, _texture{context, static_cast<const MaterialImpl*>(_material)->texture(), static_cast<const MaterialImpl*>(_material)->texture_filter()}
	{
	}

	void PushMaterial::set_uniform(const std::string& name, const Matrix4& value)
	{
		const_cast<MaterialImpl*>(static_cast<const MaterialImpl*>(_material))->gpu_program().set_uniform(name, value); // TODO: Remove 'const_cast'.
	}

	PushTexture::PushTexture(RenderContext& context, const Texture2D* texture, Flags<Texture2D::Filter> filter)
		: RenderModifier{context}
		, _filter{static_cast<RenderContextImpl&>(_context).push_texture(texture, filter)}
	{
	}

	PushTexture::~PushTexture() noexcept
	{
		static_cast<RenderContextImpl&>(_context).pop_texture(_filter);
	}

	PushTransformation::PushTransformation(RenderContext& context, const Matrix4& matrix)
		: RenderModifier{context}
	{
		static_cast<RenderContextImpl&>(_context).push_transformation(matrix);
	}

	PushTransformation::~PushTransformation() noexcept
	{
		static_cast<RenderContextImpl&>(_context).pop_transformation();
	}
}
