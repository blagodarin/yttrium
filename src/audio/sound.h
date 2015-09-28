#ifndef _src_audio_sound_h_
#define _src_audio_sound_h_

#include <yttrium/audio/sound.h>

#include <yttrium/string.h>

namespace Yttrium
{
	class AudioReader;

	class SoundImpl : public Sound
	{
	public:

		const String& name() const { return _name; }

		virtual bool load(AudioReader& reader) = 0;

	protected:

		SoundImpl(const StaticString& name, Allocator* allocator): Sound(allocator), _name(name, allocator) {}

	private:

		const String _name;
	};
}

#endif
