#ifndef _src_renderer_material_h_
#define _src_renderer_material_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/renderer/material.h>
#include <yttrium/resources/resource_ptr.h>

namespace Yttrium
{
	class GpuProgram;
	class ResourceLoader;
	class StaticString;
	class Texture2D;

	class MaterialImpl : public Material
	{
	public:
		MaterialImpl(ResourceLoader&, const StaticString&);

		GpuProgram& gpu_program() noexcept { return *_program; }
		const GpuProgram& gpu_program() const noexcept { return *_program; }
		const Texture2D* texture() const noexcept { return _texture.get(); }

	protected:
		ResourcePtr<const Texture2D> _texture; // TODO: Texture filter.
		UniquePtr<GpuProgram> _program;
	};
}

#endif
