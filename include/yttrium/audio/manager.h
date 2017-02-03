/// \file
/// \brief

#ifndef _include_yttrium_audio_manager_h_
#define _include_yttrium_audio_manager_h_

#include <yttrium/api.h>

#include <memory>

namespace Yttrium
{
	class Reader;
	template <typename> class ResourcePtr;
	class Sound;

	/// Audio manager.
	class Y_API AudioManager
	{
	public:
		///
		AudioManager();

		///
		~AudioManager();

		///
		ResourcePtr<Sound> create_sound(Reader&&);

	private:
		const std::unique_ptr<class AudioManagerPrivate> _private;
		friend class AudioPlayer;
	};
}

#endif
