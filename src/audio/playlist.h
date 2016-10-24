#ifndef _src_audio_playlist_h_
#define _src_audio_playlist_h_

#include <yttrium/audio/player.h>
#include <yttrium/std/vector.h>
#include <yttrium/string.h>

#include <mutex>

namespace Yttrium
{
	class AudioPlaylist
	{
	public:

		struct Item
		{
			String                name;
			AudioPlayer::Settings settings;

			Item(Allocator& allocator)
				: name(&allocator)
			{
			}

			Item(const String& name, const AudioPlayer::Settings& settings)
				: name(name)
				, settings(settings)
			{
			}
		};

		AudioPlaylist(Allocator&);

		void clear();
		void load(const StaticString& name, const AudioPlayer::Settings&);
		bool next(Item* item);
		AudioPlayer::Order order();
		void set_order(AudioPlayer::Order order);

	private:

		Allocator&         _allocator;
		std::mutex         _mutex;
		StdVector<Item>    _items;
		AudioPlayer::Order _order = AudioPlayer::Loop;
		size_t             _next = 0;
	};
}

#endif
