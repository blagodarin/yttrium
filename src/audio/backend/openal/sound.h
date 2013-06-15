#ifndef __AUDIO_BACKEND_OPENAL_SOUND_H
#define __AUDIO_BACKEND_OPENAL_SOUND_H

#include "../../sound.h"

#include "format.h"

namespace Yttrium
{

class OpenAlSound
	: public Sound::Private
{
public:

	OpenAlSound(const StaticString &name, Allocator *allocator);

	~OpenAlSound() override;

public: // Sound::Private

	bool load(AudioReader *reader) override;

	void play() override;

private:

	ALuint       _source;
	ALuint       _buffer;
	OpenAlFormat _format;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_SOUND_H
