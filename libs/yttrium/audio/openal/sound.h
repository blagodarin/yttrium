#ifndef _src_audio_openal_sound_h_
#define _src_audio_openal_sound_h_

#include <yttrium/audio/sound.h>
#include "format.h"
#include "wrappers.h"

namespace Yttrium
{
	class OpenALSound final : public Sound
	{
	public:
		explicit OpenALSound(class AudioReader&);

		void play() const override;

	private:
		OpenALFormat _format;
		OpenALSource _source;
		OpenALBuffer _buffer;
	};
}

#endif