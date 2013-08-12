/// \file
/// \brief Sounds.

#ifndef __Y_AUDIO_SOUND_H
#define __Y_AUDIO_SOUND_H

#include <Yttrium/object.h>
#include <Yttrium/string.h>

namespace Yttrium
{

class Sound;

/// Sound pointer.

typedef ObjectPointer<Sound> SoundPtr;

/// Sound.

class Y_API Sound
	: public Object
{
public:

	/// Return the sound name.
	/// \return %Sound name.

	String name() const noexcept { return _name; }

	/// Play the sound.

	virtual void play() const noexcept = 0;

public:

	/// Open a sound.
	/// \param name %Sound name.
	/// \param allocator %Allocator.
	/// \return %Sound pointer.

	static SoundPtr open(const StaticString &name, Allocator *allocator = nullptr) noexcept;

protected:

	Sound(const StaticString &name, Allocator *allocator) noexcept: Object(allocator), _name(name, allocator) {}

protected:

	const String _name;
};

} // namespace Yttrium

#endif // __Y_AUDIO_SOUND_H
