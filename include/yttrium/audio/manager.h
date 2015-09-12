/// \file
/// \brief Audio player.

#ifndef __Y_AUDIO_MANAGER_H
#define __Y_AUDIO_MANAGER_H

#include <yttrium/static_string.h>

#include <vector>

namespace Yttrium
{
	class AudioPlayer;
	template <typename> class Pointer;

	/// Audio manager.
	class Y_API AudioManager
	{
	public:

		///
		static std::vector<StaticString> backends();

		///
		static std::vector<StaticString> backend_devices(const StaticString& backend);

		///
		static Pointer<AudioManager> create(const StaticString& backend = StaticString(),
			const StaticString& device = StaticString(), Allocator* allocator = nullptr);

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
