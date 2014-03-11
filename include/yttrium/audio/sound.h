/// \file
/// \brief Sounds.

#ifndef __Y_AUDIO_SOUND_H
#define __Y_AUDIO_SOUND_H

#include <yttrium/pointer.h>
#include <yttrium/string.h>

namespace Yttrium
{

class Sound;

/// Sound pointer.

typedef Pointer<Sound> SoundPtr;

/// Sound.

class Y_API Sound: public Pointable
{
public:

	~Sound() = default;

	/// Return the sound name.
	/// \return %Sound name.

	String name() const noexcept
	{
		return _name;
	}

	/// Play the sound.

	virtual void play() const noexcept = 0;

public:

	/// Open a sound.
	/// \param name %Sound name.
	/// \param allocator %Allocator.
	/// \return %Sound pointer.

	static SoundPtr open(const StaticString &name, Allocator *allocator = nullptr) noexcept;

protected:

	Sound(const StaticString &name, Allocator *allocator) noexcept
		: Pointable(allocator)
		, _name(name, allocator)
	{
	}

protected:

	const String _name;
};

} // namespace Yttrium

#endif // __Y_AUDIO_SOUND_H
