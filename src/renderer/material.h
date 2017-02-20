#ifndef _src_renderer_material_h_
#define _src_renderer_material_h_

#include <yttrium/renderer/material.h>

#include <memory>

namespace Yttrium
{
	class GpuProgram;
	class Texture2D;

	class MaterialImpl : public Material
	{
	public:
		MaterialImpl(std::unique_ptr<GpuProgram>&&, std::shared_ptr<const Texture2D>&&);

		GpuProgram& gpu_program() noexcept { return *_program; }
		const GpuProgram& gpu_program() const noexcept { return *_program; }
		const Texture2D* texture() const noexcept { return _texture.get(); }

	protected:
		const std::unique_ptr<GpuProgram> _program;
		const std::shared_ptr<const Texture2D> _texture; // TODO: Texture filter.
	};
}

#endif
