#include "music.h"

#include <yttrium/audio/reader.h>
#include <yttrium/storage/reader.h>

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

	std::unique_ptr<Music> Music::open(Reader&& reader)
	{
		return reader ? std::make_unique<MusicImpl>(AudioReader::open(std::move(reader))) : nullptr;
	}
}
