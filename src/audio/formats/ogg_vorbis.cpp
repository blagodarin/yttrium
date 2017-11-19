#include "../../config.h"
#include "ogg_vorbis.h"

#include <yttrium/exceptions.h>
#include <yttrium/utils.h>

#include <algorithm>
#include <cstring>

namespace
{
	size_t read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		return static_cast<Yttrium::Reader*>(datasource)->read(ptr, size * nmemb);
	}

	int seek_callback(void* datasource, ogg_int64_t offset, int whence)
	{
		auto& reader = *static_cast<Yttrium::Reader*>(datasource);
		switch (whence)
		{
		case SEEK_SET: return reader.seek(static_cast<uint64_t>(offset)) ? 0 : -1;
		case SEEK_CUR: return reader.skip(static_cast<uint64_t>(offset)) ? 0 : -1;
		case SEEK_END: return reader.seek(reader.size() + static_cast<uint64_t>(offset)) ? 0 : -1;
		}
		return -1;
	}

	int close_callback(void*)
	{
		return 0;
	}

	long tell_callback(void* datasource)
	{
		return static_cast<long>(static_cast<Yttrium::Reader*>(datasource)->offset());
	}

	const ov_callbacks y_ov_callbacks =
	{
		read_callback,
		seek_callback,
		close_callback,
		tell_callback,
	};
}

namespace Yttrium
{
	OggVorbisReader::OggVorbisReader(std::unique_ptr<Source>&& source)
		: AudioReaderImpl{std::move(source)}
	{
		::memset(&_ov_file, 0, sizeof _ov_file);
		if (::ov_open_callbacks(&_reader, &_ov_file, nullptr, 0, ::y_ov_callbacks) < 0)
			throw DataError("Bad Ogg Vorbis file");

		vorbis_info* info = ::ov_info(&_ov_file, -1);
		_format = AudioFormat{2, static_cast<size_t>(info->channels), static_cast<size_t>(info->rate)};
		_total_samples = static_cast<uint64_t>(::ov_pcm_total(&_ov_file, -1));
	}

	OggVorbisReader::~OggVorbisReader()
	{
		::ov_clear(&_ov_file);
	}

	size_t OggVorbisReader::read(void* buffer, size_t bytes_to_read)
	{
		const size_t block_size = _format.block_size();
		bytes_to_read = std::min<uint64_t>(bytes_to_read / block_size, _total_samples - _current_sample) * block_size;
		size_t bytes_read = 0;
		for (int bitstream = 0; bytes_read <= bytes_to_read; )
		{
			const auto size = static_cast<int>(std::min(bytes_to_read - bytes_read, static_cast<size_t>(std::numeric_limits<int>::max())));
			const auto read = ::ov_read(&_ov_file, static_cast<char*>(buffer) + bytes_read, size, 0, 2, 1, &bitstream);
			if (read <= 0)
				break;
			bytes_read += static_cast<size_t>(read);
		}
		_current_sample += bytes_read / block_size;
		return bytes_read;
	}

	bool OggVorbisReader::seek(uint64_t block_offset)
	{
		if (block_offset > _total_samples)
			return false;
		if (::ov_pcm_seek(&_ov_file, static_cast<ogg_int64_t>(block_offset)) != 0)
			return false;
		_current_sample = block_offset;
		return true;
	}
}
