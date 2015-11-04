/// \file
/// \brief Sounds.

#ifndef _include_yttrium_audio_sound_h_
#define _include_yttrium_audio_sound_h_

#include <yttrium/object.h>

namespace Yttrium
{
	class StaticString;

	/// %Sound.
	class Y_API Sound : public Object
	{
	public:

		/// Creates a sound.
		static SharedPtr<Sound> create(const StaticString& name, Allocator* allocator = nullptr);

		~Sound() override = default;

		/// Plays the sound asynchronously.
		virtual void play() const = 0;

	protected:
		Sound(Allocator* allocator) : Object(allocator) {}
	};
}

#endif
