#include <yttrium/renderer/modifiers.h>

#include <yttrium/math/matrix.h>
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

	Push3D::Push3D(Renderer& renderer, const Matrix4& matrix)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_projection_3d(matrix);
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

	PushTexture::PushTexture(Renderer& renderer, const Texture2D* texture)
		: RendererModifier(renderer)
	{
		static_cast<RendererImpl&>(_renderer).push_texture(texture);
	}

	PushTexture::~PushTexture()
	{
		static_cast<RendererImpl&>(_renderer).pop_texture();
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
