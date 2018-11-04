#pragma once

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	class WasapiSound final : public Sound
	{
	public:
		explicit WasapiSound(class AudioReader&);

		void play() const override;
	};
}
