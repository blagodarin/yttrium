#ifndef __AUDIO_PLAYLIST_HPP
#define __AUDIO_PLAYLIST_HPP

#include <vector> // std::vector

#include <cstdlib> // rand

#include <Yttrium/audio/player.hpp>
#include <Yttrium/string.hpp>

#include "../base/mutex.hpp"

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

		Item()
		{
		}

		Item(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type)
			: name(name)
			, settings(settings)
			, type(type)
		{
		}
	};

public:

	AudioPlaylist(Allocator *allocator);

public:

	void clear();

	void load(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type);

	bool next(Item *item);

	AudioPlayer::Order order();

	void set_order(AudioPlayer::Order order);

private:

	typedef std::vector<Item> Items;

	StaticMutex        _mutex;
	Items              _items;
	AudioPlayer::Order _order;
	size_t             _next;
};

} // namespace Yttrium

#endif // __AUDIO_PLAYLIST_HPP
