#ifndef _src_audio_playlist_h_
#define _src_audio_playlist_h_

#include <yttrium/audio/player.h>
#include <yttrium/std/vector.h>
#include <yttrium/string.h>

#include <memory>
#include <mutex>

namespace Yttrium
{
	class AudioReader;

	class AudioPlaylist
	{
	public:
		AudioPlaylist(Allocator&);

		void clear();
		void load(Reader&&, const AudioPlayer::Settings&);
		std::pair<std::shared_ptr<AudioReader>, AudioPlayer::Settings> next();
		AudioPlayer::Order order();
		void set_order(AudioPlayer::Order order);

	private:
		Allocator&         _allocator;
		std::mutex         _mutex;
		StdVector<std::pair<std::shared_ptr<AudioReader>, AudioPlayer::Settings>> _items;
		AudioPlayer::Order _order = AudioPlayer::Loop;
		size_t             _next = 0;
	};
}

#endif
