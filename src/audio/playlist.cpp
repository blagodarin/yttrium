#include "playlist.h"

#include <yttrium/audio/reader.h>

#include <cstdlib>

namespace Yttrium
{
	AudioPlaylist::AudioPlaylist(Allocator& allocator)
		: _allocator(allocator)
		, _items(_allocator)
	{
	}

	void AudioPlaylist::clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_items.clear();
		_next = 0;
	}

	void AudioPlaylist::load(Reader&& reader, const AudioPlayer::Settings& settings)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		auto audio_reader = AudioReader::open(std::move(reader));
		if (!audio_reader)
			return;
		_items.emplace_back(std::move(audio_reader), settings);
		if (_order == AudioPlayer::Loop)
			_next = _items.size() - 1;
	}

	std::pair<std::shared_ptr<AudioReader>, AudioPlayer::Settings> AudioPlaylist::next()
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (_items.empty())
			return {};

		switch (_order)
		{
		case AudioPlayer::Loop:
			_next = (_next + 1) % _items.size();
			break;

		case AudioPlayer::Random:
		case AudioPlayer::Shuffle:
			_next = std::rand() % _items.size();
			break;
		}

		return _items[_next];
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
}
