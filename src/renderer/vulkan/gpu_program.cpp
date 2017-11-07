#include "gpu_program.h"

#include <yttrium/math/matrix.h>
#include "renderer.h"

namespace Yttrium
{
	void VulkanGpuProgram::set_uniform(const std::string&, const Matrix4& matrix)
	{
		_renderer.update_uniforms(&matrix, sizeof matrix); // TODO: Fix this hack.
	}
}
