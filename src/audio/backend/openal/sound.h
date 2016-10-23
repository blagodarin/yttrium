#ifndef _src_audio_backend_openal_sound_h_
#define _src_audio_backend_openal_sound_h_

#include "../../sound.h"
#include "format.h"

namespace Yttrium
{
	class OpenAlSound : public SoundImpl
	{
	public:
		~OpenAlSound() override;

		void play() const override;

	private:
		bool load(AudioReader&) override;

	private:
		ALuint _source = 0;
		ALuint _buffer = 0;
		OpenAlFormat _format;
	};
}

#endif
