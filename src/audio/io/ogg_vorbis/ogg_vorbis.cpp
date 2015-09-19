#include "ogg_vorbis.h"

#include <algorithm>
#include <cstring>

namespace Yttrium
{
	namespace
	{
		size_t _ov_read_callback(void* ptr, size_t size, size_t nmemb, void* datasource)
		{
			return static_cast<File*>(datasource)->read(ptr, size * nmemb); // TODO: Check if the product fits into size_t.
		}

		int _ov_seek_callback(void* datasource, ogg_int64_t offset, int whence)
		{
			return (static_cast<File*>(datasource)->seek(offset, File::Whence(whence)) ? 0 : -1);
		}

		int _ov_close_callback(void*)
		{
			return 0;
		}

		long _ov_tell_callback(void* datasource)
		{
			return static_cast<File*>(datasource)->offset();
		}

		const ov_callbacks _ov_callbacks =
		{
			_ov_read_callback,
			_ov_seek_callback,
			_ov_close_callback,
			_ov_tell_callback
		};
	}

	OggVorbisReader::OggVorbisReader(const StaticString& name, Allocator* allocator)
		: AudioReaderImpl(name, allocator)
	{
		::memset(&_ov_file, 0, sizeof _ov_file);
	}

	OggVorbisReader::~OggVorbisReader()
	{
		::ov_clear(&_ov_file);
	}

	bool OggVorbisReader::open()
	{
		if (::ov_open_callbacks(&_file, &_ov_file, nullptr, 0, _ov_callbacks) < 0)
			return false;

		vorbis_info* info = ::ov_info(&_ov_file, -1);

		_format.bytes_per_sample = 2;
		_format.channels = info->channels;
		_format.frequency = info->rate;
		_total_units = ::ov_pcm_total(&_ov_file, -1);

		return true;
	}

	size_t OggVorbisReader::read(void* buffer, size_t bytes_to_read)
	{
		const size_t unit_size = _format.unit_size();
		bytes_to_read = std::min<uint64_t>(bytes_to_read / unit_size, _total_units - _offset_units) * unit_size;
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
