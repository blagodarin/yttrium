#ifndef __AUDIO_SOUND_H
#define __AUDIO_SOUND_H

#include <yttrium/audio/sound.h>

namespace Yttrium
{
	class AudioReader;

	class SoundImpl: public Sound
	{
	public:

		virtual bool load(AudioReader* reader) = 0;

	protected:

		SoundImpl(const StaticString& name, Allocator* allocator): Sound(name, allocator) {}
	};
}

#endif // __AUDIO_SOUND_H
