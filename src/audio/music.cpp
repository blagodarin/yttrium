#include "music.h"

#include <yttrium/audio/reader.h>

#include <cassert>

namespace Yttrium
{
	MusicImpl::MusicImpl(std::unique_ptr<AudioReader>&& reader)
		: _reader(std::move(reader))
	{
		assert(_reader);
	}

	void MusicImpl::set_settings(const Settings& settings)
	{
		_settings = settings;
	}

	std::unique_ptr<Music> Music::open(std::unique_ptr<Source>&& source)
	{
		return source ? std::make_unique<MusicImpl>(AudioReader::open(std::move(source))) : nullptr;
	}
}
