#ifndef __AUDIO_SOUND_H
#define __AUDIO_SOUND_H

#include <yttrium/audio/sound.h>

#include <yttrium/string.h>

namespace Yttrium
{
	class AudioReader;

	class SoundImpl : public Sound
	{
	public:

		String name() const { return _name; }

		virtual bool load(AudioReader& reader) = 0;

	protected:

		SoundImpl(const StaticString& name, Allocator* allocator): Sound(allocator), _name(name, allocator) {}

	private:

		const String _name;
	};
}

#endif // __AUDIO_SOUND_H
