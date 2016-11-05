#include "ogg_vorbis.h"

#include <yttrium/utils.h>

#include <cstring>

namespace
{
	using namespace Yttrium;

	size_t read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
	{
		return static_cast<Reader*>(datasource)->read(ptr, size * nmemb); // TODO: Check if the product fits into size_t.
	}

	int seek_callback(void* datasource, ogg_int64_t offset, int whence)
	{
		auto& reader = *static_cast<Reader*>(datasource);
		switch (whence)
		{
		case SEEK_SET: return reader.seek(offset) ? 0 : -1;
		case SEEK_CUR: return reader.skip(offset) ? 0 : -1;
		case SEEK_END: return reader.seek(reader.size() + offset) ? 0 : -1;
		}
		return -1;
	}

	int close_callback(void*)
	{
		return 0;
	}

	long tell_callback(void* datasource)
	{
		return static_cast<Reader*>(datasource)->offset();
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
	OggVorbisReader::OggVorbisReader(Reader&& reader)
		: AudioReaderImpl(std::move(reader))
	{
		::memset(&_ov_file, 0, sizeof _ov_file);
	}

	OggVorbisReader::~OggVorbisReader()
	{
		::ov_clear(&_ov_file);
	}

	bool OggVorbisReader::open()
	{
		if (::ov_open_callbacks(&_reader, &_ov_file, nullptr, 0, ::y_ov_callbacks) < 0)
			return false;

		vorbis_info* info = ::ov_info(&_ov_file, -1);

		_format = AudioFormat(2, info->channels, info->rate);
		_total_units = ::ov_pcm_total(&_ov_file, -1);

		return true;
	}

	size_t OggVorbisReader::read(void* buffer, size_t bytes_to_read)
	{
		const size_t unit_size = _format.unit_size();
		bytes_to_read = min<uint64_t>(bytes_to_read / unit_size, _total_units - _offset_units) * unit_size;
		size_t bytes_read = 0;
		for (int bitstream = 0; bytes_read <= bytes_to_read; )
		{
			long read = ::ov_read(&_ov_file, (char*)buffer + bytes_read, bytes_to_read - bytes_read, 0, 2, 1, &bitstream);
			if (read <= 0)
				break;
			bytes_read += read;
		}
		_offset_units += bytes_read / unit_size;
		return bytes_read;
	}

	bool OggVorbisReader::seek(uint64_t offset_units)
	{
		if (offset_units > _total_units)
			return false;
		if (::ov_pcm_seek(&_ov_file, offset_units) != 0)
			return false;
		_offset_units = offset_units;
		return true;
	}
}
