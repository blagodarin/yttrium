#include <yttrium/renderer/modifiers.h>

#include <yttrium/math/matrix.h>
#include <yttrium/renderer/gpu_program.h>
#include <yttrium/renderer/texture.h>
#include "material.h"
#include "renderer.h"

namespace Yttrium
{
	Push2D::Push2D(Renderer& renderer)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_projection_2d(Matrix4::projection_2d(_renderer.window_size()));
	}

	Push2D::~Push2D()
	{
		static_cast<RendererImpl&>(_renderer).pop_projection();
	}

	Push3D::Push3D(Renderer& renderer, const Matrix4& projection, const Matrix4& view)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_projection_3d(projection, view);
	}

	Push3D::~Push3D()
	{
		static_cast<RendererImpl&>(_renderer).pop_projection();
	}

	PushGpuProgram::PushGpuProgram(Renderer& renderer, const GpuProgram* program)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_program(program);
	}

	PushGpuProgram::~PushGpuProgram()
	{
		static_cast<RendererImpl&>(_renderer).pop_program();
	}

	PushMaterial::PushMaterial(Renderer& renderer, const Material* material)
		: RendererModifier(renderer)
		, _material(material)
		, _gpu_program(renderer, &static_cast<const MaterialImpl*>(_material)->gpu_program())
		, _texture(renderer, static_cast<const MaterialImpl*>(_material)->texture(), static_cast<Texture2D::Filter>(Texture2D::NearestFilter | Texture2D::AnisotropicFilter))
	{
	}

	void PushMaterial::set_uniform(const StaticString& name, const Matrix4& value)
	{
		const_cast<MaterialImpl*>(static_cast<const MaterialImpl*>(_material))->gpu_program().set_uniform(name, value); // TODO: Remove 'const_cast'.
	}

	PushTexture::PushTexture(Renderer& renderer, const Texture2D* texture, Texture2D::Filter filter)
		: RendererModifier(renderer)
		, _filter(static_cast<RendererImpl&>(_renderer).push_texture(texture, filter))
	{
	}

	PushTexture::~PushTexture()
	{
		static_cast<RendererImpl&>(_renderer).pop_texture(_filter);
	}

	PushTransformation::PushTransformation(Renderer& renderer, const Matrix4& matrix)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_transformation(matrix);
	}

	PushTransformation::~PushTransformation()
	{
		static_cast<RendererImpl&>(_renderer).pop_transformation();
	}
}
