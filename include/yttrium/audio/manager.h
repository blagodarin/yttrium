/// \file
/// \brief Audio player.

#ifndef _include_yttrium_audio_manager_h_
#define _include_yttrium_audio_manager_h_

#include <yttrium/static_string.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class AudioPlayer;
	template <typename> class Pointer;

	/// Audio manager.
	class Y_API AudioManager
	{
	public:

		///
		static StdVector<StaticString> backends(Allocator& allocator = *DefaultAllocator);

		///
		static StdVector<StaticString> backend_devices(const StaticString& backend, Allocator& allocator = *DefaultAllocator);

		///
		static Pointer<AudioManager> create(const StaticString& backend = StaticString(),
			const StaticString& device = StaticString(), Allocator& allocator = *DefaultAllocator);

		AudioManager() = default;
		virtual ~AudioManager() = default;

		///
		/// Get the current backend name.
		/// \return Audio backend name.
		virtual StaticString backend() const = 0;

		/// Get the current backend device name.
		/// \return Audio output device name.
		virtual StaticString device() const = 0;

		///
		virtual AudioPlayer& player() = 0;
	};
}

#endif
