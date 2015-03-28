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
	String name() const { return _name; }

	/// Play the sound.
	virtual void play() const = 0;

public:

	/// Open a sound.
	/// \param name %Sound name.
	/// \param allocator %Allocator.
	/// \return %Sound pointer.
	static SoundPtr open(const StaticString& name, Allocator* allocator = nullptr);

protected:

	const String _name;

	Sound(const StaticString &name, Allocator *allocator)
		: Pointable(allocator)
		, _name(name, allocator)
	{
	}
};

} // namespace Yttrium

#endif // __Y_AUDIO_SOUND_H
