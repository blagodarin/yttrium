#include "playlist.h"

#include <cstdlib> // rand

namespace Yttrium
{

AudioPlaylist::AudioPlaylist(Allocator *allocator)
	: _allocator(allocator)
	, _order(AudioPlayer::Loop)
	, _next(0)
{
}

void AudioPlaylist::clear()
{
	std::lock_guard<std::mutex> lock(_mutex);

	_items.clear();
	_next = 0;
}

void AudioPlaylist::load(const StaticString &name, const AudioPlayer::Settings &settings, AudioType type)
{
	std::lock_guard<std::mutex> lock(_mutex);

	_items.push_back(Item(String(name, _allocator), settings, type));

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
	std::lock_guard<std::mutex> lock(_mutex);

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
	std::lock_guard<std::mutex> lock(_mutex);

	return _order;
}

void AudioPlaylist::set_order(AudioPlayer::Order order)
{
	std::lock_guard<std::mutex> lock(_mutex);

	_order = order;
}

} // namespace Yttrium
