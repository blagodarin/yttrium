#ifndef _src_audio_manager_h_
#define _src_audio_manager_h_

#include <yttrium/audio/manager.h>

#include "player.h"

namespace Yttrium
{
	class AudioBackend;

	class AudioManagerPrivate
	{
	public:
		AudioManagerPrivate(const Storage&, Allocator&);

	private:
		const Storage& _storage;
		Allocator& _allocator;
		const std::unique_ptr<AudioBackend> _backend;
		AudioPlayerImpl _player;

		friend AudioManager;
	};
}

#endif
