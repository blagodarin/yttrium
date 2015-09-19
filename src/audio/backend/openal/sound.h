#ifndef _src_audio_backend_openal_sound_h_
#define _src_audio_backend_openal_sound_h_

#include "../../sound.h"
#include "format.h"

namespace Yttrium
{
	class OpenAlBackend;

	class OpenAlSound : public SoundImpl
	{
		friend OpenAlBackend;

	public:

		void play() const override;

	private:

		OpenAlSound(const StaticString& name, Allocator* allocator)
			: SoundImpl(name, allocator)
		{
		}

		~OpenAlSound() override;

		bool load(AudioReader&) override;

	private:

		ALuint       _source = 0;
		ALuint       _buffer = 0;
		OpenAlFormat _format;
	};
}

#endif
