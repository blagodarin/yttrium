/// \file
/// \brief Sounds.

#ifndef __Y_AUDIO_SOUND_H
#define __Y_AUDIO_SOUND_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

class AudioManager;

/// Sound.

class Y_API Sound
{
	friend AudioManager;

public:

	///

	inline Sound() noexcept;

	///

	Sound(const Sound &sound) noexcept;

	///

	~Sound() noexcept;

public:

	///

	inline bool is_valid() const noexcept;

	///

	void play() const noexcept;

public:

	///

	Sound &operator =(const Sound &sound) noexcept;

public:

	class Private;

private:

	Private *_private;

private:

	Sound(Private *private_)
		: _private(private_)
	{
	}
};

Sound::Sound() noexcept
	: _private(nullptr)
{
}

bool Sound::is_valid() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_AUDIO_SOUND_H
