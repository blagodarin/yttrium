#include "playlist.h"

#include <cstdlib> // rand

namespace Yttrium
{

AudioPlaylist::AudioPlaylist(Allocator *)
	: _order(AudioPlayer::Loop)
	, _next(0)
{
}

void AudioPlaylist::clear()
{
	Mutex::Locker locker(&_mutex);

	_items.clear();
	_next = 0;
}

void AudioPlaylist::load(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type)
{
	Mutex::Locker locker(&_mutex);

	_items.push_back(Item(name, settings, type));

	switch (_order)
	{
	case AudioPlayer::Random:
	case AudioPlayer::Shuffle:

		_next = rand() % _items.size();

	default:

		_next = 0;
	}
}

bool AudioPlaylist::next(Item *item)
{
	Mutex::Locker locker(&_mutex);

	if (_items.empty())
	{
		return false;
	}

	*item = _items[_next];

	switch (_order)
	{
	case AudioPlayer::Loop:

		_next = (_next + 1) % _items.size();
		break;

	case AudioPlayer::Random:
	case AudioPlayer::Shuffle:

		_next = rand() % _items.size();
		break;
	}

	return true;
}

AudioPlayer::Order AudioPlaylist::order()
{
	Mutex::Locker locker(&_mutex);

	return _order;
}

void AudioPlaylist::set_order(AudioPlayer::Order order)
{
	Mutex::Locker locker(&_mutex);

	_order = order;
}

} // namespace Yttrium
