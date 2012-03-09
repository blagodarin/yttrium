#include "ogg_vorbis.hpp"

#include <Yttrium/file_system.hpp>

namespace Yttrium
{

namespace
{

size_t _ov_read_callback(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return static_cast<File *>(datasource)->read(ptr, size * nmemb); // NOTE: Overflow?
}

int _ov_seek_callback(void *datasource, ogg_int64_t offset, int whence)
{
	return (static_cast<File *>(datasource)->seek(offset, File::Whence(whence)) ? 0 : -1);
}

int _ov_close_callback(void *datasource)
{
	return 0;
}

long _ov_tell_callback(void *datasource)
{
	return static_cast<File *>(datasource)->offset();
}

const ov_callbacks _ov_callbacks =
{
	_ov_read_callback,
	_ov_seek_callback,
	_ov_close_callback,
	_ov_tell_callback
};

} // namespace

OggVorbisReader::~OggVorbisReader()
{
	ov_clear(&_ov_file);
}

bool OggVorbisReader::open(const StaticString &name, FileSystem &file_system)
{
	_file = file_system.open_file(name);

	if (!_file || ov_open_callbacks(&_file, &_ov_file, NULL, 0, _ov_callbacks) < 0)
	{
		return false;
	}

	vorbis_info *info = ov_info(&_ov_file, -1);

	_format.depth = 2;
	_format.channels = info->channels;
	_format.frequency = info->rate;

	_size = ov_pcm_total(&_ov_file, -1);
	_atom_size = _format.atom_size();

	return true;
}

size_t OggVorbisReader::read(void *buffer, size_t bytes_to_read)
{
	size_t bytes_read = 0;

	int bitstream;

	while (bytes_read <= bytes_to_read)
	{
		long read = ov_read(&_ov_file, (char *)buffer + bytes_read, bytes_to_read - bytes_read, 0, 2, 1, &bitstream);
		if (read <= 0)
		{
			break;
		}
		bytes_read += read;
	}

	return bytes_read;
}

void OggVorbisReader::seek(UOffset offset)
{
	ov_pcm_seek(&_ov_file, offset);
}

} // namespace Yttrium
