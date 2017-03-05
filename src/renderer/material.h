#ifndef _src_renderer_material_h_
#define _src_renderer_material_h_

#include <yttrium/renderer/material.h>
#include <yttrium/renderer/texture.h>

#include <memory>

namespace Yttrium
{
	class GpuProgram;

	class MaterialImpl : public Material
	{
	public:
		MaterialImpl(std::unique_ptr<GpuProgram>&&, std::shared_ptr<const Texture2D>&&, Texture2D::Filter);

		GpuProgram& gpu_program() noexcept { return *_program; }
		const GpuProgram& gpu_program() const noexcept { return *_program; }
		const Texture2D* texture() const noexcept { return _texture.get(); }
		Texture2D::Filter texture_filter() const noexcept { return _texture_filter; }

	protected:
		const std::unique_ptr<GpuProgram> _program;
		const std::shared_ptr<const Texture2D> _texture;
		const Texture2D::Filter _texture_filter;
	};
}

#endif
