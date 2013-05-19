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

	OpenAlSound(Allocator *allocator);

	~OpenAlSound() override;

private: // Sound::Private

	bool load(AudioReader *reader) override;

	void play() override;

private:

	ALuint       _source;
	ALuint       _buffer;
	OpenAlFormat _format;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_SOUND_H
