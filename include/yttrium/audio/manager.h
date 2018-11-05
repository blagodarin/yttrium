#ifndef _include_yttrium_audio_manager_h_
#define _include_yttrium_audio_manager_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Sound;
	class Source;

	/// Audio manager.
	class Y_ENGINE_API AudioManager
	{
	public:
		///
		AudioManager();

		///
		~AudioManager() noexcept;

		///
		std::unique_ptr<Sound> create_sound(std::unique_ptr<Source>&&);

	private:
		const std::unique_ptr<class AudioManagerPrivate> _private;
		friend class AudioPlayer;
	};
}

#endif
