#ifndef __AUDIO_SOUND_H
#define __AUDIO_SOUND_H

#include <Yttrium/audio/sound.h>
#include <Yttrium/string.h>

#include "../base/private_base.h"

namespace Yttrium
{

class AudioReader;

class Y_PRIVATE Sound::Private
	: public PrivateBase<Sound::Private>
{
public:

	Private(const StaticString &name, Allocator *allocator)
		: PrivateBase(allocator)
		, _name(name, allocator)
	{
	}

	virtual ~Private() {}

public:

	virtual bool load(AudioReader *reader) = 0;

	inline String name() const;

	virtual void play() = 0;

private:

	String _name;
};

String Sound::Private::name() const
{
	return _name;
}

} // namespace Yttrium

#endif // __AUDIO_SOUND_H
