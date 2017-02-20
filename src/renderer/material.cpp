#include "material.h"

#include <yttrium/renderer/gpu_program.h>

namespace Yttrium
{
	MaterialImpl::MaterialImpl(std::unique_ptr<GpuProgram>&& program, std::shared_ptr<const Texture2D>&& texture)
		: _program(std::move(program))
		, _texture(std::move(texture))
	{
	}
}
