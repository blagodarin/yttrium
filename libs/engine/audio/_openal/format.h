#pragma once

#include "../../application/openal.h"

#include <cstddef>

namespace Yttrium
{
	class OpenALFormat
	{
	public:
		ALenum _format = 0;
		ALsizei _frequency = 0;

		OpenALFormat() = default;
		explicit OpenALFormat(const class AudioFormat&);
	};
}
