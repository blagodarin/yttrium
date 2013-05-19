#ifndef __AUDIO_SOUND_H
#define __AUDIO_SOUND_H

#include <Yttrium/audio/sound.h>

#include "../base/private_base.h"

namespace Yttrium
{

class AudioReader;

class Y_PRIVATE Sound::Private
	: public PrivateBase<Sound::Private>
{
public:

	Private(Allocator *allocator)
		: PrivateBase(allocator)
	{
	}

	virtual ~Private() {}

public:

	virtual bool load(AudioReader *reader) = 0;

	virtual void play() = 0;
};

} // namespace Yttrium

#endif // __AUDIO_SOUND_H
