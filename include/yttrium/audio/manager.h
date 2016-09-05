/// \file
/// \brief Audio player.

#ifndef _include_yttrium_audio_manager_h_
#define _include_yttrium_audio_manager_h_

#include <yttrium/static_string.h>
#include <yttrium/std/vector.h>

namespace Yttrium
{
	class AudioPlayer;

	template <typename> class UniquePtr;

	/// Audio manager.
	class Y_API AudioManager
	{
	public:

		///
		static StdVector<StaticString> backends(Allocator& = *DefaultAllocator);

		///
		static StdVector<StaticString> backend_devices(const StaticString& backend, Allocator& = *DefaultAllocator);

		///
		static UniquePtr<AudioManager> create(const StaticString& backend = {}, const StaticString& device = {}, Allocator& = *DefaultAllocator);

		AudioManager() = default;
		virtual ~AudioManager() = default;

		/// Returns the current backend name.
		virtual StaticString backend() const = 0;

		/// Returns the current backend device name.
		virtual StaticString device() const = 0;

		///
		virtual AudioPlayer& player() = 0;
	};
}

#endif
