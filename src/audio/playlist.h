#ifndef __AUDIO_PLAYLIST_H
#define __AUDIO_PLAYLIST_H

#include <yttrium/audio/player.h>
#include <yttrium/string.h>

#include <mutex>
#include <vector>

namespace Yttrium
{
	class AudioPlaylist
	{
	public:

		struct Item
		{
			String                name;
			AudioPlayer::Settings settings;
			AudioType             type;

			Item(Allocator* allocator)
				: name(allocator)
			{
			}

			Item(const String& name, const AudioPlayer::Settings& settings, AudioType type)
				: name(name)
				, settings(settings)
				, type(type)
			{
			}
		};

		AudioPlaylist(Allocator* allocator);

		void clear();
		void load(const StaticString& name, const AudioPlayer::Settings& settings, AudioType type);
		bool next(Item* item);
		AudioPlayer::Order order();
		void set_order(AudioPlayer::Order order);

	private:

		Allocator*         _allocator;
		std::mutex         _mutex;
		std::vector<Item>  _items;
		AudioPlayer::Order _order;
		size_t             _next;
	};
}

#endif // __AUDIO_PLAYLIST_H
