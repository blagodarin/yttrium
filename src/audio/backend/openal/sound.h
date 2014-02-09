#ifndef __AUDIO_BACKEND_OPENAL_SOUND_H
#define __AUDIO_BACKEND_OPENAL_SOUND_H

#include "../../sound.h"
#include "format.h"

namespace Yttrium
{

class OpenAlManager;

class OpenAlSound
	: public SoundImpl
{
	friend OpenAlManager;

public:

	void play() const noexcept override;

private:

	OpenAlSound(const StaticString &name, Allocator *allocator)
		: SoundImpl(name, allocator), _source(0), _buffer(0) {}

	~OpenAlSound() noexcept override;

	bool load(AudioReader *reader) override;

private:

	ALuint       _source;
	ALuint       _buffer;
	OpenAlFormat _format;
};

} // namespace Yttrium

#endif // __AUDIO_BACKEND_OPENAL_SOUND_H
