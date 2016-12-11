#ifndef _examples_3d_material_h_
#define _examples_3d_material_h_

#include <yttrium/memory/unique_ptr.h>
#include <yttrium/renderer/modifiers.h>
#include <yttrium/resources/resource_ptr.h>

namespace Yttrium
{
	class GpuProgram;
	class ResourceLoader;
	class StaticString;
	class Texture2D;
}

using namespace Yttrium;

class Material
{
public:
	Material(ResourceLoader&, const StaticString&);
	~Material();

	GpuProgram& gpu_program() noexcept { return *_program; }
	const GpuProgram& gpu_program() const noexcept { return *_program; }
	const Texture2D* texture() const noexcept { return _texture.get(); }

protected:
	ResourcePtr<const Texture2D> _texture; // TODO: Texture filter.
	UniquePtr<GpuProgram> _program;
};

class PushMaterial
{
public:
	PushMaterial(Renderer& renderer, const Material& material)
		: _gpu_program(renderer, &material.gpu_program())
		, _texture(renderer, material.texture(), static_cast<Texture2D::Filter>(Texture2D::NearestFilter | Texture2D::AnisotropicFilter))
	{
	}

private:
	PushGpuProgram _gpu_program;
	PushTexture _texture;
};

#endif
