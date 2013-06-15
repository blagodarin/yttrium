/// \file
/// \brief Sounds.

#ifndef __Y_AUDIO_SOUND_H
#define __Y_AUDIO_SOUND_H

#include <Yttrium/static_string.h>

namespace Yttrium
{

/// Sound.

class Y_API Sound
{
public:

	///

	inline Sound() noexcept;

	///

	inline Sound(const StaticString &name, Allocator *allocator = nullptr) noexcept;

	///

	Sound(const Sound &sound) noexcept;

	///

	inline ~Sound() noexcept;

public:

	///

	void close() noexcept;

	///

	inline bool is_opened() const noexcept;

	///

	bool open(const StaticString &name, Allocator *allocator = nullptr) noexcept;

	///

	void play() const noexcept;

public:

	///

	Sound &operator =(const Sound &sound) noexcept;

public:

	class Private;

private:

	Private *_private;
};

Sound::Sound() noexcept
	: _private(nullptr)
{
}

Sound::Sound(const StaticString &name, Allocator *allocator) noexcept
	//: Sound() // TODO: Uncomment.
	: _private(nullptr)
{
	open(name, allocator);
}

Sound::~Sound() noexcept
{
	close();
}

bool Sound::is_opened() const noexcept
{
	return _private;
}

} // namespace Yttrium

#endif // __Y_AUDIO_SOUND_H
